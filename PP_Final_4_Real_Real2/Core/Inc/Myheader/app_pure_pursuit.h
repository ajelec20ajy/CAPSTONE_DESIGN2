/*
 * pure_pursuit.h
 *
 *  Created on: Nov 1, 2025
 *      Author: ajy97
 */

#ifndef SRC_CONTROL_APP_PURE_PURSUIT_H_
#define SRC_CONTROL_APP_PURE_PURSUIT_H_

#include "types.h"
void PP_Init(PP_Handle_t* handle, Waypoint_t* waypoints, int count); // 웨이포인트 설정값들 불러들이기
ControlCommand_t PP_Compute(PP_Handle_t* handle, RobotPose_t* pose); // Pure-Pursuit 기반 주행 명령(조향, 감/가속)

#endif /* SRC_CONTROL_APP_PURE_PURSUIT_H_ */
