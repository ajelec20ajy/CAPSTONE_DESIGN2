/*
 * bsp_lcd.h
 *
 *  Created on: Jan 22, 2026
 *      Author: ajy97
 */

#ifndef SRC_BSP_ETC_BSP_LCD_H_
#define SRC_BSP_ETC_BSP_LCD_H_
#include <stdint.h>

void lcd_buf_clear(void); // 버퍼초기화
void lcd_buf_push(uint8_t value, uint8_t rs_bit); // 버퍼채우기
void lcd_flush_dma(void); // 내보내기!
void lcd_send_cmd(char cmd); // 커맨드보내기

#endif /* SRC_BSP_ETC_BSP_LCD_H_ */
