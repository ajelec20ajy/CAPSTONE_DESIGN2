/*
 * encoder.h
 *
 *  Created on: Nov 1, 2025
 *      Author: ajy97
 */

#ifndef SRC_HARDWARE_APP_ENCODER_H_
#define SRC_HARDWARE_APP_ENCODER_H_
#include "bsp_encoder.h"
float Encoder_GetDist(); // 직진 주행 거리반환(두 모터 평균값)
float Encoder_GetDist_R(); // 우측 모터 주행 거리
float Encoder_GetDist_L(); // 좌측 모터 주행 거리
#endif /* SRC_HARDWARE_APP_ENCODER_H_ */
