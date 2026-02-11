/*
 * encoder.c
 *
 *  Created on: Nov 1, 2025
 *      Author: ajy97
 */
#include "types.h"
#include "bsp_encoder.h"


float Straight_Distance = 0; // 직진 주행 거리(두 모터 평균)
static float Straight_Distance_R = 0; // 우측 모터 직진 주행 거리
static float Straight_Distance_L = 0; // 좌측 모터 직진 주행 거리

uint16_t debug1; // CNT 디버깅용-우측
uint16_t debug2; // CNT 디버깅용-좌측

void Encoder_UpdateDistance(){
	// 누적 카운트들
    static int32_t total_cnt_R = 0;
    static int32_t total_cnt_L = 0;
    // 각각의 이전 값
    static uint16_t last_cnt1 = 0; // R
    static uint16_t last_cnt2 = 0; // L

    // 현재 값 읽기
    uint16_t curr1 = GET_CNT1();
    uint16_t curr2 = GET_CNT2();

    // 걍 카운터 Live로 볼려고
    debug1 = curr1;
    debug2 = curr2;
    // 변화량 계산
    // (int16_t) 캐스팅을 통해 wrap-around 자동 처리
    int16_t diff1 = (int16_t)(curr1 - last_cnt1); // R
    int16_t diff2 = (int16_t)(curr2 - last_cnt2); // L

    // 변화량 반영
    total_cnt_R += diff1;
    total_cnt_L += diff2;
    last_cnt1 = curr1;
    last_cnt2 = curr2;

    // 주행 거리 계산 : 거리 = (cnt / 한바퀴당 회전 수) * 바퀴 길이
    Straight_Distance_L = ((float)total_cnt_L / (float)COUNTS_PER_REV_L) * CIRCUMFERENCE_M;
    Straight_Distance_R = ((float)total_cnt_R / (float)COUNTS_PER_REV_R) * CIRCUMFERENCE_M;

    // 거리 변환
    Straight_Distance = (Straight_Distance_L+Straight_Distance_R) / 2.0f; // 평균으로 사용
}

float Encoder_GetDist(){ // 직진 주행 거리 계산 후반환
	Encoder_UpdateDistance();
	return Straight_Distance;
}
float Encoder_GetDist_R(){ // 그냥 만들어둠. 모터우측 주행거리 반환
	return Straight_Distance_R;
}
float Encoder_GetDist_L(){ // 그냥 만들어줌. 좌측 모터 주행거리 반환
	return Straight_Distance_L;
}


