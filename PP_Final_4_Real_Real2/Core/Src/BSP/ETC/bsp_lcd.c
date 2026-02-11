/*
 * bsp_lcd.c
 *
 *  Created on: Jan 22, 2026
 *      Author: ajy97
 */

#include <stm32f3xx_hal.h> // I2C
#include "cmsis_os2.h" // Semaphore 사용을 위해 추가
#define LCD_ADDR (0x27 << 1) // 주소 확인 필요
#define LCD_DMA_BUF_SIZE  256 // 넉넉하게 잡음

extern I2C_HandleTypeDef hi2c2;

// DMA 전송을 위한 세마포어와 버퍼
extern osSemaphoreId_t lcdI2cSemHandle;
static uint8_t lcd_dma_buf[256];
static uint16_t lcd_buf_idx = 0;

// 전송 완료 콜백
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c) {
    if (hi2c->Instance == I2C2) { // lcd 모듈은 i2c2
        osSemaphoreRelease(lcdI2cSemHandle);
    }
}

// 버퍼 초기화
void lcd_buf_clear(void) {
    lcd_buf_idx = 0;
}

// 버퍼에 4-bit 패킷(4바이트)
void lcd_buf_push(uint8_t value, uint8_t rs_bit) {
    uint8_t u = (value & 0xf0);
    uint8_t l = ((value << 4) & 0xf0);
    uint8_t mode = rs_bit ? 0x01 : 0x00; // RS 설정 (Data: 0x01, Cmd: 0x00)

    // 버퍼 넘침 방지 체크 (한 번에 4바이트씩 추가되므로)
    if (lcd_buf_idx + 4 > 256) return;

    // 상위 4비트 전송 패킷
    lcd_dma_buf[lcd_buf_idx++] = u | 0x0C | mode; // EN=1, Backlight=1
    lcd_dma_buf[lcd_buf_idx++] = u | 0x08 | mode; // EN=0, Backlight=1

    // 하위 4비트 전송 패킷
    lcd_dma_buf[lcd_buf_idx++] = l | 0x0C | mode; // EN=1, Backlight=1
    lcd_dma_buf[lcd_buf_idx++] = l | 0x08 | mode; // EN=0, Backlight=1
}

// 쌓인 버퍼를 DMA로 전송
void lcd_flush_dma(void) {
    if (lcd_buf_idx == 0) return;
    if (HAL_I2C_Master_Transmit_DMA(&hi2c2, LCD_ADDR, lcd_dma_buf, lcd_buf_idx) == HAL_OK) {
        osSemaphoreAcquire(lcdI2cSemHandle, osWaitForever); // 전송 중 태스크 Block
    }
    lcd_buf_idx = 0;
}

void lcd_send_cmd(char cmd) {
    char data_u = (cmd & 0xf0);
    char data_l = ((cmd << 4) & 0xf0);
    uint8_t data_t[4] = {data_u|0x0C, data_u|0x08, data_l|0x0C, data_l|0x08};
    HAL_I2C_Master_Transmit(&hi2c2, LCD_ADDR, data_t, 4, 100);
}
