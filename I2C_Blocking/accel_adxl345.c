/*
 * accel_adxl345.c
 *
 *  Created on: Aug 30, 2025
 *      Author: ajy97
 */
#include "accel_adxl345.h"
#include <string.h>
#include <math.h>

#define M_PI 3.14159265358979323846

float ax_sum_adxl = 0, ay_sum_adxl = 0, az_sum_adxl = 0;
static Data_Accel_ADXL adxlOffset;
Data_Accel_ADXL adxldata;
float ax_filt_prev_adxl = 0, ay_filt_prev_adxl = 0, az_filt_prev_adxl = 0;

void ADXL_Init(void){
    uint8_t data;

    HAL_Delay(50);
    // 1) BW_RATE 설정: 100Hz (rate code 0x0A 권장 예시)
    data = 0x0A;
    I2C1_WriteReg(ADXL345_I2C_ADDR, ADXL_REG_BW_RATE, data);

    // 2) DATA_FORMAT: FULL_RES(=1) + range = 0 (±2g)
    // FULL_RES bit = D3 -> 0x08
    data = 0x08;
    I2C1_WriteReg(ADXL345_I2C_ADDR, ADXL_REG_DATA_FORMAT, data);

    // 3) POWER_CTL: Measure bit (D3) = 1 (기기 측정 모드로)
    data = 0x08;
    I2C1_WriteReg(ADXL345_I2C_ADDR, ADXL_REG_POWER_CTL, data);

    data = 0x00;
    I2C1_WriteReg(ADXL345_I2C_ADDR, 0x38, data);


    // 짧게 안정화 시간
    HAL_Delay(20);


    // 버퍼용 초기 드랍
    for(int i=0;i<50;i++){
        ADXL_ReadRaw(&adxldata);
    }
    for(int i=0;i<1000;i++){
        ADXL_ReadRaw(&adxldata);
		ax_sum_adxl += adxldata.x;
		ay_sum_adxl += adxldata.y;
		az_sum_adxl += adxldata.z;
    }
	adxlOffset.x = ax_sum_adxl / 1000;
	adxlOffset.y = ay_sum_adxl / 1000;
	adxlOffset.z = az_sum_adxl / 1000;
}

void ADXL_ReadRaw(Data_Accel_ADXL *d){
    uint8_t buf[6];
    I2C1_ReadMulti(ADXL345_I2C_ADDR, ADXL_REG_DATAX0, buf, 6);
    // buf[0]=DATAX0 (LSB), buf[1]=DATAX1(MSB)
    d->x = (int16_t)((buf[1] << 8) | buf[0]);
    d->y = (int16_t)((buf[3] << 8) | buf[2]);
    d->z = (int16_t)((buf[5] << 8) | buf[4]);
}

void ADXL_Print(Data_Accel_ADXL *adxldata, float *ax, float *ay, float *az){
    //  offset 제거 후 g 단위로 변환
    // full-resolution mode에서 scale = 4 mg/LSB (0.004 g/LSB)
    float scale_g_per_lsb = 0.004f; // 4 mg/LSB
    float ax_g = ((float)(adxldata->x - adxlOffset.x)) * scale_g_per_lsb;
    float ay_g = ((float)(adxldata->y - adxlOffset.y)) * scale_g_per_lsb;
    float az_g = ((float)(adxldata->z - adxlOffset.z)) * scale_g_per_lsb;

    // LPF-EMA 적용 (g 단위에서). 
    float alpha = 0.95f; // 
    ax_filt_prev_adxl = alpha * ax_filt_prev_adxl + (1.0f - alpha) * ax_g;
    ay_filt_prev_adxl = alpha * ay_filt_prev_adxl + (1.0f - alpha) * ay_g;
    az_filt_prev_adxl = alpha * az_filt_prev_adxl + (1.0f - alpha) * az_g;

    *ax = ax_filt_prev_adxl;
    *ay = ay_filt_prev_adxl;
    *az = az_filt_prev_adxl;
}
