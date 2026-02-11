/*
 * servo.h
 *
 *  Created on: Jan 16, 2026
 *      Author: ajy97
 */

#ifndef SRC_HARDWARE_SERVO_H_
#define SRC_HARDWARE_SERVO_H_

#include <stdint.h>

void Init_Servo(); // 서보용 tim2 pwm on
void CHANGE_SERVO_PWM(uint16_t ccr); // pwm ccr 변경하기(범위 제한)
uint32_t radian_to_servo_ccr(float rad); // Pure-Pursuit에서 지령으로 만든 조향각 라디안을 각도로 변환

#endif /* SRC_HARDWARE_SERVO_H_ */
