/*
 * mag.c
 *
 *  Created on: Aug 21, 2025
 *      Author: ajy97
 */

#include "bsp_mag.h"
#include <stm32f3xx_hal.h>
#include <math.h>

extern I2C_HandleTypeDef hi2c1;

uint8_t mag_buf[6];
float mOffset[3] = {0, 0, 0};      // 하드 아이언
float Scale[3]   = {1, 1, 1};      // 소프트 아이언
int16_t xmin = 32767, xmax = -32768; // 오프셋 저장할거
int16_t ymin = 32767, ymax = -32768;
int16_t zmin = 32767, zmax = -32768;

// 시작 각도를 0으로 만들기 위한 회전 행렬 계수
static float rot_cos = 1.0f;
static float rot_sin = 0.0f;

void Init_Mag(){
    uint8_t data;

    // 소프트 리셋
    data = 0x80; // Soft Reset
    HAL_I2C_Mem_Write(&hi2c1, QMC5883L_ADDR, QMC5883L_REG_CTRL2, I2C_MEMADD_SIZE_8BIT, &data, 1, HAL_MAX_DELAY);
    HAL_Delay(50);

    // 리셋 후 Period 설정
    data = 0x01;
    HAL_I2C_Mem_Write(&hi2c1, QMC5883L_ADDR, QMC5883L_REG_PERIOD, I2C_MEMADD_SIZE_8BIT, &data, 1, HAL_MAX_DELAY);

    // CTRL1 설정
    data = 0x05;
    HAL_I2C_Mem_Write(&hi2c1, QMC5883L_ADDR, QMC5883L_REG_CTRL1,
                      I2C_MEMADD_SIZE_8BIT, &data, 1, HAL_MAX_DELAY);
    // 약 750번 반복
    for(int i = 0; i < 750; i++) { // 오프셋 측정
    	// DMA 말고 직접 읽기
    	if(HAL_I2C_Mem_Read(&hi2c1, QMC5883L_ADDR, QMC5883L_REG_X_LSB, 1, mag_buf, 6, 100) == HAL_OK) {

    		int16_t raw_x = (int16_t)((mag_buf[1] << 8) | mag_buf[0]);
    		int16_t raw_y = (int16_t)((mag_buf[3] << 8) | mag_buf[2]);
    		int16_t raw_z = (int16_t)((mag_buf[5] << 8) | mag_buf[4]);

    		// Min/Max 갱신해서 캘리브레이션의 기반인 타원 만들기
    		if (raw_x < xmin) xmin = raw_x;
    		if (raw_x > xmax) xmax = raw_x;
    		if (raw_y < ymin) ymin = raw_y;
    		if (raw_y > ymax) ymax = raw_y;
    		if (raw_z < zmin) zmin = raw_z;
    		if (raw_z > zmax) zmax = raw_z;
    	}
        HAL_Delay(20); // 20ms 대기 (50Hz 주기 맞춤)
    }

    // 보정값(Hard Iron & Soft Iron) 계산
    // Hard Iron (Offset)
    mOffset[0] = (xmax + xmin) / 2.0f;
    mOffset[1] = (ymax + ymin) / 2.0f;
    mOffset[2] = (zmax + zmin) / 2.0f;

    // Soft Iron (Scale)
    float avg_rad_x = (xmax - xmin) / 2.0f;
    float avg_rad_y = (ymax - ymin) / 2.0f;
    float avg_rad_z = (zmax - zmin) / 2.0f;
    float avg_rad = (avg_rad_x + avg_rad_y + avg_rad_z) / 3.0f;

    // 0으로 나누기 방지
    if(avg_rad_x != 0) Scale[0] = avg_rad / avg_rad_x;
    if(avg_rad_y != 0) Scale[1] = avg_rad / avg_rad_y;
    if(avg_rad_z != 0) Scale[2] = avg_rad / avg_rad_z;

    // 시작 각도를 0도로 맞추기
    int16_t last_x = (int16_t)((mag_buf[1] << 8) | mag_buf[0]);
    int16_t last_y = (int16_t)((mag_buf[3] << 8) | mag_buf[2]);

    // 현재 방향의 벡터 보정된 벡터
    float temp_x = (float)(last_x - mOffset[0]) * Scale[0];
    float temp_y = (float)(last_y - mOffset[1]) * Scale[1];

    // 현재 각도 계산
    float start_theta = atan2f(temp_y, temp_x);

    // 이 각도만큼 반대로 회전
    rot_cos = cosf(start_theta);
    rot_sin = sinf(start_theta);
}
void Read_Mag(void) { // i2c dma로 읽어오기
    HAL_I2C_Mem_Read_DMA(&hi2c1, QMC5883L_ADDR, QMC5883L_REG_X_LSB,
                         I2C_MEMADD_SIZE_8BIT, mag_buf, 6);
}

void Mag_ProcessData(IMU_Handle_t *pIMU) { // 보정 및 IMU 업데이트
	// raw 긁어오기
	int16_t raw_x = (int16_t)((mag_buf[1] << 8) | mag_buf[0]);
	int16_t raw_y = (int16_t)((mag_buf[3] << 8) | mag_buf[2]);
	int16_t raw_z = (int16_t)((mag_buf[5] << 8) | mag_buf[4]);

	// 로우 업데이트(디버깅)
	pIMU->mag.raw_x = raw_x;
	pIMU->mag.raw_y = raw_y;
	pIMU->mag.raw_z = raw_z;

	// 캘리브레이션
	float cal_x = (float)(raw_x - mOffset[0]) * Scale[0]; // 오프셋 제거 (하드아이언 + 소프트아이언 보정)
	float cal_y = (float)(raw_y - mOffset[1]) * Scale[1];
	float cal_z = (float)(raw_z - mOffset[2]) * Scale[2];

	// 시작 각도 보정 (좌표 회전), 시작이 0도이게
    float final_x = cal_x * rot_cos + cal_y * rot_sin;
    float final_y = -cal_x * rot_sin + cal_y * rot_cos;


    // imu 구조체에 반영
	pIMU->mag.x = final_x;
	pIMU->mag.y = final_y;
	pIMU->mag.z = cal_z; // 어짜피 안씀
}
