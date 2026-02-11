#include <math.h>
#include "app_pure_pursuit.h"
#include "types.h"



float rpm_target = 0.0f; // 주행 rpm
extern PID_Config motor_pid_R; // 구간마다 가변할건데, 속도에 따라 Kp,Ki 세팅이 다름.
extern PID_Config motor_pid_L;

#define RPM_MIN_LD  			40.0f  	// 이 속도 이하일 때 최소 Ld
#define RPM_MAX_LD  			80.0f  	// 이 속도 이상일 때 최대 Ld
#define LD_MIN     				0.35f   // 최소 룩어헤드 (코너링)
#define LD_MAX      			0.8f   	// 최대 룩어헤드 (직진 주행)
#define WP_PASS_RADIUS          0.2f  	// 이 거리 안이면 다음 경유지로 넘어감
#define WP_STOP_RADIUS          0.03f 	// 정지거리 3cm
#define STEER_GAIN              1.0f	// 조향 약하게 하고 싶음 이거 줄이면댐
#define MAX_STEER_DEG           30.0f 	// 최대 조향각 제한 (도)
#define BRAKING_DIST            0.3f  	// 40cm 전부터 다음 목표 속도로 감속 시작
#define FINAL_STOP_DIST         0.2f  	// 마지막 정지 지점 30cm 전부터 0으로 감속
// Breaking_Dist 키우면 더 멀리서부터 천천히 감속, 줄이면 급감속
// LD_Min : 코너링 중 경로 이탈 시 LD_MIN 줄이기

static PID_Zone_t pid_zone = PID_ZONE_LOW; // PID세팅할것

// 히스테리시스 임계값
#define LOW_TO_MID_UP     55.0f
#define MID_TO_LOW_DOWN   50.0f

#define MID_TO_HIGH_UP    77.0f
#define HIGH_TO_MID_DOWN  73.0f

// 속도 프로파일링 함수
static float calculate_smooth_speed(float dist_to_target, float start_v, float target_v, uint8_t is_stop) {
    float current_v = start_v;

    // 최종 목적지 정지 로직
    if (is_stop) {
        if (dist_to_target < FINAL_STOP_DIST) {
            // 남은 거리에 비례하여 0으로 감속
            current_v = target_v * (dist_to_target / FINAL_STOP_DIST);
            if (current_v < 15.0f && dist_to_target > WP_STOP_RADIUS) current_v = 15.0f; // 최소 구동력 보장
        } else {
            current_v = target_v;
        }
    }
    // 일반 구간 (다음 WP 속도가 현재보다 낮을 때 미리 감속)
    else if (target_v < start_v) {
        if (dist_to_target < BRAKING_DIST) {
            // 거리 비율만큼 속도를 낮춤
            float ratio = dist_to_target / BRAKING_DIST;
            current_v = target_v + (start_v - target_v) * ratio;
        } else {
            current_v = start_v;
        }
    }
    // 가속 구간 (즉시 목표 속도 설정 - 가속도는 모터 PID가 담당)
    else {
        current_v = target_v;
    }

    return current_v;
}

// 룩 어헤드 포인트 찾기(원그리기)
static void get_lookahead_point(float rx, float ry, float sx, float sy, float ex, float ey, float ld_val, float* lx, float* ly) {
    float d_x = ex - sx;
    float d_y = ey - sy;
    float f_x = sx - rx;
    float f_y = sy - ry;

    float a = d_x*d_x + d_y*d_y;
    float b = 2.0f * (f_x*d_x + f_y*d_y);
    // 여기서 인자로 받은 ld_val 사용
    float c = (f_x*f_x + f_y*f_y) - (ld_val * ld_val);

    float discriminant = b*b - 4.0f*a*c;
    float t = 1.0f; // 기본값 End Point

    if (discriminant >= 0) {
        discriminant = sqrtf(discriminant);
        float t1 = (-b - discriminant) / (2.0f * a);
        float t2 = (-b + discriminant) / (2.0f * a);

        if (t2 >= 0.0f && t2 <= 1.0f) t = t2;
        else if (t1 >= 0.0f && t1 <= 1.0f) t = t1;
    }

    *lx = sx + t * d_x;
    *ly = sy + t * d_y;
}

void PP_Init(PP_Handle_t* handle, Waypoint_t* waypoints, int count) {
    handle->wps = waypoints;
    handle->total_wps = count;
    handle->current_idx = 0;
    handle->last_wp_x = 0.0f; // 초기 위치 혹은 첫 WP
    handle->last_wp_y = 0.0f;
}

ControlCommand_t PP_Compute(PP_Handle_t* handle, RobotPose_t* pose) {
    ControlCommand_t cmd = {0};
    static float last_dist = 999.0f;

    if (handle->current_idx >= handle->total_wps) {
        cmd.target_motor_rpm = 0.0f;
        cmd.e_stop = 1;
        return cmd;
    }

    // 현재 목표 WP와 이전 WP 정보 가져오기
    Waypoint_t target = handle->wps[handle->current_idx];
    float prev_v = (handle->current_idx == 0) ? 0.0f : handle->wps[handle->current_idx - 1].target_speed;

    float dx = target.x - pose->x;
    float dy = target.y - pose->y;
    float dist = sqrtf(dx*dx + dy*dy);

    // [1] 경유지 및 정지 판정
    if (target.is_stop_point) {
        if (dist < WP_STOP_RADIUS) {
            cmd.target_motor_rpm = 0.0f;
            cmd.target_motor_rpm_L = 0.0f;
            cmd.target_motor_rpm_R = 0.0f;
            cmd.e_stop = 1;
            return cmd;
        }
        // 오버슈트 방지
        if (dist < 0.2f && dist > last_dist + 0.02f) {
            cmd.target_motor_rpm = 0.0f;
            cmd.target_motor_rpm_L = 0.0f;
            cmd.target_motor_rpm_R = 0.0f;
            cmd.e_stop = 1;
            return cmd;
        }
        last_dist = dist;
    } else {
        if (dist < WP_PASS_RADIUS) {
            handle->last_wp_x = target.x;
            handle->last_wp_y = target.y;
            handle->current_idx++;
            // 인덱스 업데이트 후 거리 재계산 생략 (다음 주기에서 처리됨)
            return cmd;
        }
    }

    // [2] 속도 프로파일링 수행 (dist를 기반으로 현재 RPM 결정)
    rpm_target = calculate_smooth_speed(dist, prev_v, target.target_speed, target.is_stop_point);

    // [3] 가변 Ld 계산 (현재 계산된 rpm_target 반영)
    float current_ld = LD_MAX;
    if (rpm_target <= RPM_MIN_LD) {
        current_ld = LD_MIN;
    } else if (rpm_target >= RPM_MAX_LD) {
        current_ld = LD_MAX;
    } else {
        float speed_ratio = (rpm_target - RPM_MIN_LD) / (RPM_MAX_LD - RPM_MIN_LD);
        current_ld = LD_MIN + (LD_MAX - LD_MIN) * speed_ratio;
    }

    // [4] 조향 및 Lookahead Point 계산
    float lh_x, lh_y;
    get_lookahead_point(pose->x, pose->y, handle->last_wp_x, handle->last_wp_y, target.x, target.y, current_ld, &lh_x, &lh_y);

    float alpha = atan2f(lh_y - pose->y, lh_x - pose->x) - pose->theta_rad;
    while (alpha > M_PI) alpha -= 2.0f * M_PI;
    while (alpha < -M_PI) alpha += 2.0f * M_PI;

    float raw_steering = atanf((2.0f * WHEELBASE_M * sinf(alpha)) / current_ld);
    cmd.target_servo_angle = raw_steering * STEER_GAIN;

    // 조향 제한
    float max_steer_rad = MAX_STEER_DEG * (M_PI / 180.0f);
    if (cmd.target_servo_angle > max_steer_rad) cmd.target_servo_angle = max_steer_rad;
    if (cmd.target_servo_angle < -max_steer_rad) cmd.target_servo_angle = -max_steer_rad;

    // 곡률 계산 -> 차동 구동을 위함
    float kappa = tanf(cmd.target_servo_angle) / WHEELBASE_M;
    float W_eff = WHEELBASE_M * 1;  // 이 1: GAMMA -> 실제 차량이 느끼는 트랙폭.. 코너 덜 돌면 키우고, 불안정하면 줄인다.
    float k = kappa * W_eff * 0.5f; // 곡률
    if(rpm_target < RPM_DIFF_ENABLE_THRESHOLD) { // 저속에서 k = 0
        k = 0.0f;
    } // 이미 cmd.target_servo_anlge이 클램핑 되어있기에 따로 클램핑안함

    // PID 게인 설정 (히스테리시스)
    switch (pid_zone)
    {
    case PID_ZONE_LOW:
        if (rpm_target > LOW_TO_MID_UP) {
            pid_zone = PID_ZONE_MID;
        }
        break;

    case PID_ZONE_MID:
        if (rpm_target < MID_TO_LOW_DOWN) {
            pid_zone = PID_ZONE_LOW;
        }
        else if (rpm_target > MID_TO_HIGH_UP) {
            pid_zone = PID_ZONE_HIGH;
        }
        break;

    case PID_ZONE_HIGH:
        if (rpm_target < HIGH_TO_MID_DOWN) {
            pid_zone = PID_ZONE_MID;
        }
        break;
    }

    // PID 게인 적용
    if (pid_zone == PID_ZONE_LOW) {
        motor_pid_R.Kp = 0.637f; motor_pid_R.Ki = 0.085f;
        motor_pid_L.Kp = 0.637f; motor_pid_L.Ki = 0.083f;
    }
    else if (pid_zone == PID_ZONE_MID) {
        motor_pid_R.Kp = 0.872f; motor_pid_R.Ki = 0.106f;
        motor_pid_L.Kp = 0.791f; motor_pid_L.Ki = 0.115f;
    }
    else { // PID_ZONE_HIGH
        motor_pid_R.Kp = 1.082f; motor_pid_R.Ki = 0.082f;
        motor_pid_L.Kp = 1.07f; motor_pid_L.Ki = 0.109f;
    }

    cmd.target_motor_rpm = rpm_target;
    cmd.k = k;
    cmd.target_motor_rpm_L = rpm_target * (1.0f - k);
    cmd.target_motor_rpm_R = rpm_target * (1.0f + k);
    return cmd;
}
