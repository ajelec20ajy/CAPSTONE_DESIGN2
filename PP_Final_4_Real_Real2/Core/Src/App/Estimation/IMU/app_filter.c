/*
 * filter.c
 *
 *  Created on: Nov 1, 2025
 *      Author: ajy97
 */
#define ALPHA_CF 1.0F
#include "types.h" // 구조체
#include <math.h> // atan2f 등

#define ALPHA       0.957f // 엑셀 파일 참조
#define BETA		0.005f // acc lpf beta

static float Normalize_Angle(float angle) { // 래핑 함수
    while (angle > M_PI) { // 180보다 크면
        angle -= (2.0f * M_PI); // -180으로 넘겨줌
    }

    while (angle < -M_PI) { // -180보다 작으면
        angle += (2.0f * M_PI); // 180으로 넘겨줌
    }

    return angle;
}

void Tilt_Compensation(IMU_Handle_t *pIMU){ // 지자기 센서의 기울어짐 보상 by 가속도기반 롤/핓치
	static float lp_ax = 0, lp_ay = 0, lp_az = 0;
	float beta = BETA; // 파이썬에서 전송받은 값 (0.0 ~ 1.0)

	// 가속도 LPF 강하게 걸어도 되는듯 어짜피 기울기가 잘 안바뀜
	lp_ax = (pIMU->acc.x * beta) + (lp_ax * (1.0f - beta));
	lp_ay = (pIMU->acc.y * beta) + (lp_ay * (1.0f - beta));
	lp_az = (pIMU->acc.z * beta) + (lp_az * (1.0f - beta));

    // 지자기
    float mx = pIMU->mag.x;
    float my = pIMU->mag.y;
    float mz = pIMU->mag.z;

    // 롤/핓치 계산(가속도값)
    float roll  = atan2f(lp_ay, lp_az);
    float pitch = atan2f(-lp_ax, sqrtf(lp_ay * lp_ay + lp_az * lp_az));

    // 걍 나중에 제어에 쓸수도? 있으니깐 구조체에 저장해줌
    pIMU->roll_rad  = roll;
    pIMU->pitch_rad = pitch;

    // Tilt Compensation - 공식 from 구글링
    // 삼각함수 미리 계산 (CPU 부하 줄이기)
    float cos_roll  = cosf(roll);
    float sin_roll  = sinf(roll);
    float cos_pitch = cosf(pitch);
    float sin_pitch = sinf(pitch);
    float X_h = mx * cos_pitch + my * sin_roll * sin_pitch + mz * cos_roll * sin_pitch;
    float Y_h = my * cos_roll - mz * sin_roll;

    // 지자기 헤딩 계산
    float heading = atan2f(Y_h, X_h);

    // 구조체에 결과 업데이트
    pIMU->mag_heading = heading; // 상보필터가 가져다 쓸 값. 라디안임
}

void Complemetary_Filter(IMU_Handle_t *pIMU){ // CF 필터 연산 함수
	// 데이터 가져오기
    float gyro_rate = pIMU->gyro_z_rad_s;  // 현재 회전 속도 (rad/s)
    float mag_heading = pIMU->mag_heading; // 지자기 헤딩(기울어짐 보상 완료
    float current_yaw = pIMU->yaw_rad;     // 이전 루프의 결과값

    // 자이로 GZ 적분 -> 자이로 기반 Yaw
    float predicted_yaw = current_yaw + (gyro_rate * DT_SENSOR);
    predicted_yaw = Normalize_Angle(predicted_yaw);

    // 지자기와 자이로의 차이
    float error = mag_heading - predicted_yaw;
    error = Normalize_Angle(error); // 래핑처리 안하면(정규화) 179랑 -179가 358도 차이가 되어버렷

    // CF 적용. ALPHA가 필터 상수임.
    float fused_yaw = predicted_yaw + (error * (1.0f - ALPHA));

    // 최종 결과 정규화 및 저장
    pIMU->yaw_rad = Normalize_Angle(fused_yaw);
}

