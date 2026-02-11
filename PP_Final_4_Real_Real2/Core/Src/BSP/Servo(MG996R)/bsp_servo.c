/*
 * servo.c
 *
 *  Created on: Jan 16, 2026
 *      Author: ajy97
 */
#include "bsp_servo.h"
#include <math.h>
#include <stm32f3xx_hal.h>
#include "types.h"

extern TIM_HandleTypeDef htim2;

// 타이머 설정이 1 tick = 1us 라고 가정 (PSC 조절 필요)
#define SERVO_CENTER_US     1500  // 0도
#define SERVO_RANGE_US_PHYSICAL      500.0f
#define STEERING_MAX_DEG_PHYSCCIAL 	45.0f
// 1도당 펄스 변화량 (500us / 45도 = 약 11.1111)
#define US_PER_DEG          (SERVO_RANGE_US_PHYSICAL / STEERING_MAX_DEG_PHYSCCIAL) // 500us당 45도
// 안준- 찾아보니깐 각도랑 신호랑은 선형적이라 그냥 Factor만 넣어서 인가하면 될거같아요. 일단 찾은바로는 0.5ms에서 0도, 1.5ms에서 90도, 2.5ms에서 180도라고 하네요
// 서보 최대 30도일때 각 1167, 1833; 좌회전이 1167 // 우회전이 1833
      // 중앙 1500
#define SERVO_CENTER_US     1500  // 서보 중립 펄스 (튜닝 필요: 차가 직진하는 값)
#define SERVO_MAX_US        1833  // 우회전 30도
#define SERVO_MIN_US        1167  // 좌회전 30도
#define STEERING_MAX_DEG 	30.0f // 차량의 실제 조향 한계각 (30도)

// 현재 서보의 물리적 위치(CCR)를 기억할 정적 변수
static float current_servo_ccr = SERVO_CENTER_US;
extern TIM_HandleTypeDef htim2; // 서보모터용 타이머 핸들

void Init_Servo(){
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 1500); //전방정렬
}
void CHANGE_SERVO_PWM(uint16_t target_ccr){
	uint16_t servo_ccr = target_ccr;
    // 1. 하드웨어 보호 범위 제한
    if(servo_ccr >= SERVO_MAX_US) servo_ccr = SERVO_MAX_US;
    if(servo_ccr <= SERVO_MIN_US) servo_ccr = SERVO_MIN_US;

    // 3. 타이머 CCR 업데이트
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, (uint16_t)servo_ccr);
}

// 라디안 -> 서보 PWM CCR 값 변환
uint32_t radian_to_servo_ccr(float rad) {
    // 라디안 -> 도(Degree) 변환
    float deg = rad * Rad2Deg; // Rad2Deg = 57.295...

    // 차량 조향 한계 제한 (서보 보호가 아니라 조향 메커니즘 보호)
    if (deg > STEERING_MAX_DEG) deg = STEERING_MAX_DEG;
    if (deg < -STEERING_MAX_DEG) deg = -STEERING_MAX_DEG;

    // PWM 펄스 계산
    float pulse_us = SERVO_CENTER_US + (deg * US_PER_DEG);

    return (uint16_t)pulse_us;
}
