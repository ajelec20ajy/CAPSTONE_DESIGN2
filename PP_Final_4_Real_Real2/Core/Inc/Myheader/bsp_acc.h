/*
 * accel_adxl345.h
 *
 *  Created on: Aug 30, 2025
 *      Author: ajy97
 */

#ifndef INC_ACC_H_
#define INC_ACC_H_

#include "types.h"

#define ADXL345_I2C_ADDR     0x53<<1  // 7-bit address
// ADXL345 레지스터 (필요한 것들만)
#define ADXL_REG_DEVID       0x00
#define ADXL_REG_DATA_FORMAT 0x31
#define ADXL_REG_BW_RATE     0x2C
#define ADXL_REG_POWER_CTL   0x2D
#define ADXL_REG_DATAX0      0x32
#define ADXL_REG_OFSTX       0x1E
#define ADXL_REG_OFSTY       0x1F
#define ADXL_REG_OFSTZ       0x20
#define ADXL_REG_FIFO_CTL    0x38
#define ADXL_INT_ENABLE  0x2E
#define ADXL_INT_MAP     0x2F
#define ADXL_INT_SOURCE  0x30
// 함수
void Init_Accel(void); // 센서 초기설정 및 오프셋 캘리브레이션
void Read_Accel(void); // 값 읽어오기 DMA 요청
void Accel_ProcessData(IMU_Handle_t *pIMU); // 오프셋, 스케일 처리 등

#endif /* INC_ACC_H_ */
