/*
 * filter.h
 *
 *  Created on: Nov 1, 2025
 *      Author: ajy97
 */

#ifndef SRC_ESTIMATION_FILTER_H_
#define SRC_ESTIMATION_FILTER_H_

void Tilt_Compensation(IMU_Handle_t *pIMU); // 기울어짐 보상(지자기 <- 가속도)

void Complemetary_Filter(IMU_Handle_t *pIMU); // 상보필터

#endif /* SRC_ESTIMATION_FILTER_H_ */
