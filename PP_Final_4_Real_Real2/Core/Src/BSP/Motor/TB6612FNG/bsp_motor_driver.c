/*
 * motor.c
 *
 *  Created on: Nov 1, 2025
 *      Author: ajy97
 */
#include "bsp_motor_driver.h"
#include "main.h" // htim이랑 AIN1, AIN2 핀등
extern TIM_HandleTypeDef htim1;

#define PWM_MAX 999 // 최대 pwm 듀티

void Motor_Drive_TIM1_L(int16_t pwm) { // 좌측 모터 드라이버 PWM 인가하기 -PWMA(PA8, TIM1_CH1)
	int16_t ccr = pwm; // 그냥보기 편하게
    if (ccr > 999) ccr = 999;
    if (ccr < -999) ccr = -999;

	if (ccr >= 0) {
    // 정방향
    HAL_GPIO_WritePin(AIN1_GPIO_Port, AIN1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(AIN2_GPIO_Port, AIN2_Pin, GPIO_PIN_RESET);
	}

	else {
    // 역방향 (브레이크/감속)
    HAL_GPIO_WritePin(AIN1_GPIO_Port, AIN1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(AIN2_GPIO_Port, AIN2_Pin, GPIO_PIN_SET);
    ccr = -ccr; // PWM 값은 양수로 변환하여 인가
	}

	  HAL_GPIO_WritePin(STBY_GPIO_Port, STBY_Pin, GPIO_PIN_SET); // STBY. 매번해서나쁠건 없음

	  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, (uint32_t)ccr); // 듀티 수정해서 PWM신호인가
}
void Motor_Drive_TIM1_R(int16_t pwm) { // 우측은 PWMB(PA9, TIM1_CH2)
	int16_t ccr = pwm;
    if (ccr > 999) ccr = 999;
    if (ccr < -999) ccr = -999;
    if (ccr >= 0) {
    // 정방향
    HAL_GPIO_WritePin(BIN1_GPIO_Port, BIN1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(BIN2_GPIO_Port, BIN2_Pin, GPIO_PIN_SET);
	}
	else {
    // 역방향 (브레이크/감속)
    HAL_GPIO_WritePin(BIN1_GPIO_Port, BIN1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(BIN2_GPIO_Port, BIN2_Pin, GPIO_PIN_RESET);
    ccr = -ccr; // PWM 값은 양수로 변환하여 인가
	}

	HAL_GPIO_WritePin(STBY_GPIO_Port, STBY_Pin, GPIO_PIN_SET); // STBY. 매번해서나쁠건 없음

	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, (uint32_t)ccr); // 듀티 수정해서 PWM신호인가
}


void Init_Motor_Driver(){
	  HAL_GPIO_WritePin(STBY_GPIO_Port, STBY_Pin, GPIO_PIN_SET); // TB6612 stby
	  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1); // PWMA 시작 (PA8)
	  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2); // PWMB 시작 (PA9)
	  HAL_GPIO_WritePin(AIN1_GPIO_Port, AIN1_Pin, GPIO_PIN_SET); // TB6612FNG 방향 설정
	  HAL_GPIO_WritePin(AIN2_GPIO_Port, AIN2_Pin, GPIO_PIN_RESET); // TB6612FNG 방향 설정
}
