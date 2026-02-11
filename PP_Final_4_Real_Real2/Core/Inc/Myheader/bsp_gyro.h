/*
 * itg3205.h
 *
 *  Created on: Sep 2, 2025
 *      Author: ajy97
 */

#ifndef INC_GYRO_H_
#define INC_GYRO_H_

#include "types.h"

// 레지스터
#define ITG3205_ADDR 0x68<<1 // i2c 주소
#define ITG3205_WHO_AM_I   0x00
#define ITG3205_PWR_MGM    0x3E
#define ITG3205_SMPLRT_DIV 0x15
#define ITG3205_DLPF_FS    0x16
#define ITG3205_GYRO_XOUT_H 0x1D

void Init_Gyro(void); // 초기화 및 오프셋 측정
void Read_Gyro(void); // DMA I2C 요청해서 센서 데이터 읽어오기
void Gyro_ProcessData(IMU_Handle_t *pIMU); // 오프셋, 스케일 등 보정해서 gx, gy, gz 및 rad/s

typedef struct {
	uint64_t gt[2];
    int16_t x;
    int16_t y;
    int16_t z;
} GyroRaw; // RAW 담을거

#endif
