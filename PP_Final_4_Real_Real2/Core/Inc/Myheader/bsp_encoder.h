/*
 * bsp_encoder.h
 *
 *  Created on: Jan 22, 2026
 *      Author: ajy97
 */

#ifndef SRC_BSP_MOTOR_ENCODER_BSP_ENCODER_H_
#define SRC_BSP_MOTOR_ENCODER_BSP_ENCODER_H_

void Init_Encoder(); //엔코더 측정 시작을 위해, 엔코더모드로 TIM들 시작
uint16_t GET_CNT1(); // 걍 함 이렇게 해봄
uint16_t GET_CNT2();

#endif /* SRC_BSP_MOTOR_ENCODER_BSP_ENCODER_H_ */
