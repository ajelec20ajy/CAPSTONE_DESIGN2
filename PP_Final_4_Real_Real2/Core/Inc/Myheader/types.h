/*
 * types.h
 *
 *  Created on: Nov 1, 2025
 *      Author: ajy97
 */

#ifndef SRC_TYPES_H_
#define SRC_TYPES_H_

#include <stdint.h>

typedef struct AccMag{
	float mx, my, mz;
	float ax, ay, az;
}AccMag;

// 상수 설정
#define DT_SENSOR           0.01f   // 센서 주기 (10ms)
#define DT_CONTROL          0.02f   // 제어 주기 (20ms)

#define WHEELBASE_M         0.15f   // 미니카 축간거리 (15cm)
#define DIAMETER_M          0.065f // 데이터 시트상 지름
#define CIRCUMFERENCE_M     (3.1415926f * DIAMETER_M) * 1.01335375053 // 바퀴 지름 보정값(2pr * 반지름)
#define COUNTS_PER_REV_R       3863.0f // 엔코더 R
#define COUNTS_PER_REV_L	   3871.0f // 엔코더 L

#define RPM_CONVERSION_FACTOR_R  60.0f / (COUNTS_PER_REV_R * DT_CONTROL) // RIGHT RPM 변환
#define RPM_CONVERSION_FACTOR_L 60.0f / (COUNTS_PER_REV_L * DT_CONTROL) // LEFT

#define Deg2Rad 0.01745329f
#define Rad2Deg 57.295779513f

#define RPM_DIFF_ENABLE_THRESHOLD 40.0f // 차동구동 시작할 RPM

// 현재 상태
typedef enum {
    STATE_IDLE = 0,    // 대기 (모터 꺼짐)
    STATE_RUNNING,     // 주행 중 (Pure Pursuit)
    STATE_FINISHED,    // 목표 도달 및 정지
} MissionState_e;

typedef struct { // 제어 명령 구조체
    float target_servo_angle; // 목표 조향각 (rad)
    float target_motor_rpm;   // 목표 모터 회전수 (RPM)
    float target_motor_rpm_R;
    float target_motor_rpm_L;
    float k;
    uint8_t e_stop;           // 비상 정지 플래그 //1이면 stop
} ControlCommand_t;

// 웨이포인트 구조체
typedef struct {
    float x;
    float y;
    float target_speed; // 해당 구간 최대 속도
    uint8_t is_stop_point; // 1이면 최종 도착지(정지), 0이면 경유지(통과)
} Waypoint_t;

// Pure Pursuit 상태 관리 핸들러
typedef struct {
    Waypoint_t* wps;        // 웨이포인트 배열 포인터
    int total_wps;        // 총 웨이포인트 수
    int current_idx;      // 현재 목표 인덱스
    // 세그먼트(구간) 관리를 위한 이전 좌표
    float last_wp_x;
    float last_wp_y;
} PP_Handle_t;

// 구조체-로봇의 물리적 상태 (SensorTask가 기록)
typedef struct {
    float x;            // 절대 좌표 X (m)
    float y;            // 절대 좌표 Y (m)
    float theta_rad;    // 헤딩 각도 (-PI ~ +PI, 라디안)
    float v_ms;         // 현재 선속도 (m/s)
    float RPM;			// RPM
} RobotPose_t;

// 구조체-제어 목표 및 명령 (NaviTask가 기록, ControlTask가 읽음)
typedef struct {
    float goal_x;       // 현재 목표 Waypoint X
    float goal_y;       // 현재 목표 Waypoint Y
    float target_speed; // 목표 속도 (m/s 또는 PWM)
    float target_steer; // 목표 조향 각도
    float k; 			// 곡률
    float rpmR;
    float rpmL;
    MissionState_e state; // 현재 미션 상태
} RobotCommand_t;

// PID 제어용
typedef enum {
    PID_ZONE_LOW = 0,
    PID_ZONE_MID,
    PID_ZONE_HIGH
} PID_Zone_t;

// PID 구조체
typedef struct {
    // 튜닝 게인
    float Kp;
    float Ki;
    float prev_error;
    // 상태 변수
    float target_rpm;     // 목표 속도
    float current_rpm;    // 현재 속도
    float integral_sum;   // 적분 누적값 (Error * dt의 합)

    // 출력 제한 (TIM CCR)
    int16_t out_pwm;      // 최종 PWM 출력값 (-1000 ~ 1000)
    int16_t max_pwm;      // PWM 최대값 (예: ARR = 999)
} PID_Config;

// 전체 공유 데이터 (Mutex 보호 대상)
typedef struct {
    RobotPose_t    pose; // 현재 상태
    RobotCommand_t cmd; // 명령값(도달 목표)
} SharedData_t;

//센서별 데이터 구조체
typedef struct {
	int16_t raw_x, raw_y, raw_z; // Debugging 용도
    float x, y, z;               // 오프셋 빼고 단위 변환한 값
} SensorData_t;

// 전체 IMU 데이터 구조체
typedef struct {
    // 하위 센서 데이터
    SensorData_t acc;
    SensorData_t gyro;
    SensorData_t mag;

    // 계산된 결과값 (filter.c가 채워넣을 곳)
    float roll_rad;
    float pitch_rad;

    float yaw_rad;       // 이게 Fused Heading
    float gyro_z_rad_s;  // 오도메트리에 쓸 각속도
    float gyro_heading;
    float mag_heading;
} IMU_Handle_t;


#endif /* SRC_TYPES_H_ */
