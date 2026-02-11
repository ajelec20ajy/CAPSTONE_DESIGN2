#include "bsp_gyro.h"
#include <stm32f3xx_hal.h>

extern I2C_HandleTypeDef hi2c1;

static GyroRaw gyroOffset;
uint8_t gyro_buf[6];


void Init_Gyro(void) { // Gyro 초기화
    HAL_Delay(50);

    // 전원 관리
    HAL_I2C_Mem_Write(&hi2c1, ITG3205_ADDR, ITG3205_PWR_MGM, I2C_MEMADD_SIZE_8BIT, (uint8_t[]){0x01}, 1, HAL_MAX_DELAY);

    // LPF 및 Full-scale 설정 1kHz
    HAL_I2C_Mem_Write(&hi2c1, ITG3205_ADDR, ITG3205_DLPF_FS, I2C_MEMADD_SIZE_8BIT, (uint8_t[]){0x1B}, 1, HAL_MAX_DELAY);

    // 샘플레이트 디바이더 200Hz
    HAL_I2C_Mem_Write(&hi2c1, ITG3205_ADDR, ITG3205_SMPLRT_DIV, I2C_MEMADD_SIZE_8BIT, (uint8_t[]){0x04}, 1, HAL_MAX_DELAY);

    // Data Ready Interrupt X
    HAL_I2C_Mem_Write(&hi2c1, ITG3205_ADDR, 0x17, I2C_MEMADD_SIZE_8BIT, (uint8_t[]){0x00}, 1, HAL_MAX_DELAY);
    HAL_Delay(20);

    // 오프셋 샘플링
    float gx_sum = 0, gy_sum = 0, gz_sum = 0;
    for (int i = 0; i < 50; i++) { // 초기값 조금 버리기. 버튼 등에 의해 흔들렸을수도
        HAL_I2C_Mem_Read(&hi2c1, ITG3205_ADDR, ITG3205_GYRO_XOUT_H, I2C_MEMADD_SIZE_8BIT, gyro_buf, 6, HAL_MAX_DELAY);
        HAL_Delay(10);
    }
    for (int i = 0; i < 1000; i++) { // 오프셋 위한 데이터 연속 측정
        HAL_I2C_Mem_Read(&hi2c1, ITG3205_ADDR, ITG3205_GYRO_XOUT_H, I2C_MEMADD_SIZE_8BIT, gyro_buf, 6, HAL_MAX_DELAY);
        gx_sum += (int16_t)((gyro_buf[0] << 8) | gyro_buf[1]);
        gy_sum += (int16_t)((gyro_buf[2] << 8) | gyro_buf[3]);
        gz_sum += (int16_t)((gyro_buf[4] << 8) | gyro_buf[5]);
        HAL_Delay(10);
    }
    gyroOffset.x = gx_sum / 1000;
    gyroOffset.y = gy_sum / 1000;
    gyroOffset.z = gz_sum / 1000;
}


void Read_Gyro(void) { // DMA I2C 읽기 호출
	HAL_I2C_Mem_Read_DMA(&hi2c1, ITG3205_ADDR, ITG3205_GYRO_XOUT_H,
	                           I2C_MEMADD_SIZE_8BIT, gyro_buf, 6);
}

void Gyro_ProcessData(IMU_Handle_t *pIMU) { // DMA 완료시 오프셋, 스케일 등 보정해서 IMU 구조체에 업데이트해줌
	// raw 긁어오기
    int16_t raw_x = (int16_t)((gyro_buf[0] << 8) | gyro_buf[1]);
    int16_t raw_y = (int16_t)((gyro_buf[2] << 8) | gyro_buf[3]);
    int16_t raw_z = (int16_t)((gyro_buf[4] << 8) | gyro_buf[5]);

    // IMU에 반영
    pIMU->gyro.raw_x = raw_x;
    pIMU->gyro.raw_y = raw_y;
    pIMU->gyro.raw_z = raw_z;

    // 캘리브
    float scale = 1.0f / 14.375f;
    float gx_dps = (raw_x - gyroOffset.x) * scale;
    float gy_dps = (raw_y - gyroOffset.y) * scale;
    float gz_dps = (raw_z - gyroOffset.z) * scale;

    // IMU 업데이트(디버깅용)
    pIMU->gyro.x = gx_dps;
    pIMU->gyro.y = gy_dps;
    pIMU->gyro.z = gz_dps;

    // 필터가 쓰는 변수(rad/s)로 변환해서 넣어주기
    // 1 dps = 0.0174533 rad/s
    pIMU->gyro_z_rad_s = gz_dps * 0.0174533f;
}

