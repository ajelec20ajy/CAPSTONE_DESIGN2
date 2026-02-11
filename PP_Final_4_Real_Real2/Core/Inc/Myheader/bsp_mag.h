/*
 * mag.h
 *
 *  Created on: Aug 21, 2025
 *      Author: ajy97
 */

#ifndef INC_MAG_H_
#define INC_MAG_H_

#include "types.h"

#define QMC5883L_REG_X_LSB   0x00
#define QMC5883L_REG_STATUS  0x06
#define QMC5883L_REG_TOUT_L  0x07
#define QMC5883L_REG_CTRL1   0x09
#define QMC5883L_REG_CTRL2   0x0A
#define QMC5883L_REG_PERIOD  0x0B
#define QMC5883L_REG_CHIPID  0x0D
#define QMC5883L_ADDR        0x0D<<1

void Init_Mag(); // 센서 초기화 및 오프셋(하드랑 소프트아이언)
void Read_Mag(void); // I2C DMA로 센서 데이터 읽어오기
void Mag_ProcessData(IMU_Handle_t *pIMU); // 보정 및 IMU 업데이트
#endif /* INC_MAG_H_ */
