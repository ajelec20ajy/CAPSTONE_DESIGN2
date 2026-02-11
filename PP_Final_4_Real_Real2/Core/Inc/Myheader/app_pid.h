/*
 * pid.h
 *
 *  Created on: Jan 19, 2026
 *      Author: ajy97
 */

#ifndef SRC_ESTIMATION_PID_H_
#define SRC_ESTIMATION_PID_H_

void PID_Compute_R(float target, float current); // 우측 PI제어
void PID_Compute_L(float target, float current); // 좌측 PI제어

#endif /* SRC_ESTIMATION_PID_H_ */
