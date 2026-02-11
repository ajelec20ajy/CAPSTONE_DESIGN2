/*
 * hardware_driver.c
 *
 *  Created on: Jan 16, 2026
 *      Author: ajy97
 */

#include <app_i2c_callback.h>
#include "cmsis_os.h"  // osThreadFlagsSet 쓰려면 필요
#include <stm32f3xx_hal.h> // I2C
#include "types.h"

extern osThreadId_t SENSORHandle;

#define FLAG_I2C_DONE 0x01        // Thread Flag 비트 정의 이게 FLAG_DMA_DONE인데 0x00000001U 똑같아

//  I2C 완료 콜백 : sensorTask에서 Get_Fused_Heading로 센서값들 다 읽어오는 부분에서, 플래그로 기다리면서 순차적으로 3가지 센서 i2c1으로 읽음
void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c) {
    if (hi2c->Instance == I2C1) {
        osThreadFlagsSet(SENSORHandle, FLAG_I2C_DONE);
    }
}

