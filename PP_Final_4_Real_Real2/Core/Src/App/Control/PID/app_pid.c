/*
 * pid.c
 *
 *  Created on: Jan 19, 2026
 *      Author: ajy97
 */
#include "app_pid.h"
#include "types.h"

extern PID_Config motor_pid_R; // main.c에 원본있음
extern PID_Config motor_pid_L;

float Kff_L = 4.301f; // 피드포워드 좌측 (PWM/RPM)
float kff_R = 4.301f; // 피드포워드 우측

void PID_Compute_R(float target, float current) { // 우측 모터용 PID 제어 연산(PI)
    float error = target - current; // RPM 차이
    float ff_out = kff_R * target; // 피듶포워드

    // P항 계산
    float p_out = motor_pid_R.Kp * error;

    // I항 계산

    motor_pid_R.integral_sum += (error * DT_CONTROL);
    float i_out = motor_pid_R.Ki * motor_pid_R.integral_sum;

    // 합산
    float pi_out = p_out + i_out; // P랑 I
    float total_out = ff_out + pi_out; // 피드포워드까지

    // 출력 제한
    if (total_out > motor_pid_R.max_pwm) { // 초과
        total_out = motor_pid_R.max_pwm;
        motor_pid_R.integral_sum -= (error * DT_CONTROL); // 출력이 포화되면 더 이상 적분값을 쌓지 않음 (Anti-Windup) -> 오버슛 방지
    }
    else if (total_out < -motor_pid_R.max_pwm) { // 초과. 참고로 짜피 후진구동은 안하고 Free-Wheeling으로 제어댐
        total_out = -motor_pid_R.max_pwm;
        motor_pid_R.integral_sum -= (error * DT_CONTROL);
    }

    motor_pid_R.out_pwm = (int16_t)total_out; // 제어 결과, 인가할 PWM CCR
}

void PID_Compute_L(float target, float current) { // 좌측 모터용 PID 제어 연산(PI)
    float error = target - current;
    float ff_out = Kff_L * target; // 피듶포워드

    float p_out = motor_pid_L.Kp * error;

    motor_pid_L.integral_sum += (error * DT_CONTROL);
    float i_out = motor_pid_L.Ki * motor_pid_L.integral_sum;

    float pi_out = p_out + i_out;
    float total_out = ff_out + pi_out;

    if (total_out > motor_pid_L.max_pwm) {
        total_out = motor_pid_L.max_pwm;
        motor_pid_L.integral_sum -= (error * DT_CONTROL);
    }
    else if (total_out < -motor_pid_L.max_pwm) {
        total_out = -motor_pid_L.max_pwm;
        motor_pid_L.integral_sum -= (error * DT_CONTROL);
    }
    motor_pid_L.out_pwm = (int16_t)total_out;
}

