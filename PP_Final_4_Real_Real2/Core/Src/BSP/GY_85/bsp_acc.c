/*
 * accel_adxl345.c
 *
 *  Created on: Aug 30, 2025
 *      Author: ajy97
 */

#include <stm32f3xx_hal.h> // I2C
#include "bsp_acc.h"

extern I2C_HandleTypeDef hi2c1;
uint8_t accel_buf[6]; // 가속도센서 데이터 I2C에서 받아올 버퍼
static float ax_offset = 0.0f;
static float ay_offset = 0.0f;
static float az_offset = 0.0f;

void Init_Accel(void) {

	  uint8_t val;
	  // 측정모드
	  val = 0x00; // 0000 1000 -> Measur만 킴.
	  HAL_I2C_Mem_Write(&hi2c1, ADXL345_I2C_ADDR, ADXL_REG_POWER_CTL, 1, &val, 1, 100);
	  HAL_Delay(10);

	  val = 0x08; // 0000 1000 -> Measur만 킴.
	  HAL_I2C_Mem_Write(&hi2c1, ADXL345_I2C_ADDR, ADXL_REG_POWER_CTL, 1, &val, 1, 100);
	  HAL_Delay(10);

	  // 데이터포맷 +-2g, FULL-RES
	  val = 0x08;
	  HAL_I2C_Mem_Write(&hi2c1, ADXL345_I2C_ADDR, ADXL_REG_DATA_FORMAT, 1, &val, 1, 100);
	  HAL_Delay(10);

	  // 100Hz - 0x0A 아 이게ㅐ 200
	  val = 0x0B; //09면 25Hz -> 1/25 = 40ms
	  HAL_I2C_Mem_Write(&hi2c1, ADXL345_I2C_ADDR, ADXL_REG_BW_RATE, 1, &val, 1, 100);
	  HAL_Delay(10);

	  // 인터럽트 Enable
	  val = 0x00;
	  HAL_I2C_Mem_Write(&hi2c1, ADXL345_I2C_ADDR, ADXL_INT_ENABLE, 1, &val, 1, 100);
	  HAL_Delay(10);

	  val = 0x08; // 0000 1000 -> Measur만 킴.
	  HAL_I2C_Mem_Write(&hi2c1, ADXL345_I2C_ADDR, ADXL_REG_POWER_CTL, 1, &val, 1, 100);
	  HAL_Delay(50);

    // 오프셋 캘리브레이션
    float ax_sum = 0, ay_sum = 0, az_sum = 0;
    for (int i = 0; i < 50; i++) { // 50 - 초기값은 버림 : 버튼 조작 등에 의해 조금 흔들렸을 수도 있어서..
        HAL_I2C_Mem_Read(&hi2c1, ADXL345_I2C_ADDR, ADXL_REG_DATAX0, I2C_MEMADD_SIZE_8BIT, accel_buf, 6, HAL_MAX_DELAY);
        HAL_Delay(10);
    }
    for (int i = 0; i < 1000; i++) { // 1000 - 초기값 버린 이후
        HAL_I2C_Mem_Read(&hi2c1, ADXL345_I2C_ADDR, ADXL_REG_DATAX0,
                         I2C_MEMADD_SIZE_8BIT, accel_buf, 6, HAL_MAX_DELAY);
        int16_t rx = (int16_t)((accel_buf[1] << 8) | accel_buf[0]); // 읽어온거 넣고
        int16_t ry = (int16_t)((accel_buf[3] << 8) | accel_buf[2]);
        int16_t rz = (int16_t)((accel_buf[5] << 8) | accel_buf[4]);
        ax_sum += rx; // 일단 다 더해
        ay_sum += ry;
        az_sum += rz;
        HAL_Delay(10);
    }
    ax_offset = ax_sum / 1000.0f; // 그리고 평균 구해서 오프셋
    ay_offset = ay_sum / 1000.0f;
    az_offset = az_sum / 1000.0f;
}

void Read_Accel(void) { // dma로 읽기
    HAL_I2C_Mem_Read_DMA(&hi2c1, ADXL345_I2C_ADDR, ADXL_REG_DATAX0,
                         I2C_MEMADD_SIZE_8BIT, accel_buf, 6);
}


void Accel_ProcessData(IMU_Handle_t *pIMU) { // 수신하고나서, 오프셋과 SCALE 등 보정해서 ax, ay, az를 IMU 구조체에 업데이트
	// RAW 긁어오기
    int16_t ax = (int16_t)((accel_buf[1] << 8) | accel_buf[0]);
    int16_t ay = (int16_t)((accel_buf[3] << 8) | accel_buf[2]);
    int16_t az = (int16_t)((accel_buf[5] << 8) | accel_buf[4]);

    //4 mg/LSB
    float scale_g_per_lsb = 0.004f;

    // 오프셋 보정 + g 변환
    float ax_g = (ax - ax_offset) * scale_g_per_lsb;
    float ay_g = (ay - ay_offset) * scale_g_per_lsb;
    float az_g = (az - az_offset) * scale_g_per_lsb;

    // IMU 구조체 업데이트
    pIMU->acc.x = ax_g;
    pIMU->acc.y = ay_g;
    pIMU->acc.z = az_g;
}
