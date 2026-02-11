/*
 * motor.h
 *
 *  Created on: Nov 1, 2025
 *      Author: ajy97
 */

#ifndef SRC_HARDWARE_MOTOR_H_
#define SRC_HARDWARE_MOTOR_H_

#include <stdint.h>

void Motor_Drive_TIM1_R(int16_t pwm); // 우측(A)
void Motor_Drive_TIM1_L(int16_t pwm); // 좌측(B)
void Init_Motor_Driver();			  // 초기화 TB6612FNG

#endif /* SRC_HARDWARE_MOTOR_H_ */
