/*
 * lcd.c
 *
 *  Created on: Jan 21, 2026
 *      Author: ajy97
 */
#include <app_lcd.h>
#include "types.h" // 구조체


extern SharedData_t g_RobotData;   // 로봇 상태 및 미션 정보
extern PP_Handle_t g_PPHandle;     // Pure Pursuit 관리자

void lcd_init(void) {
    lcd_send_cmd(0x30);
    lcd_send_cmd(0x30);
    lcd_send_cmd(0x32); // 4-bit mode
    lcd_send_cmd(0x28); // Function set
    lcd_send_cmd(0x0C); // Display on/off
    lcd_send_cmd(0x01); // Clear display
}

void lcd_put_cursor_buf(int row, int col) {
    uint8_t val = (row == 0) ? (col | 0x80) : (col | 0xC0);
    lcd_buf_push(val, 0); // RS=0 (Command)
}

void lcd_send_string_buf(char *str) {
    while (*str) {
        lcd_buf_push(*str++, 1); // RS=1 (Data)
    }
}
