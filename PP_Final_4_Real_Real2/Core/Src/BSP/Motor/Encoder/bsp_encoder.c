/*
 * bsp_encoder.c
 *
 *  Created on: Jan 22, 2026
 *      Author: ajy97
 */
#include <stm32f3xx_hal.h>
#include "bsp_encoder.h"
extern TIM_HandleTypeDef htim3; // 좌측 엔코더용
extern TIM_HandleTypeDef htim4; // 우측 엔코더용

void Init_Encoder(){ //엔코더 측정 시작을 위해, 엔코더모드로 TIM들 시작
	HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL); // 엔코더 - L
	HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL); // 엔코더 - R
}
uint16_t GET_CNT1(){
	return (uint16_t)TIM4->CNT;
}

uint16_t GET_CNT2(){
	return (uint16_t)TIM3->CNT;
}
