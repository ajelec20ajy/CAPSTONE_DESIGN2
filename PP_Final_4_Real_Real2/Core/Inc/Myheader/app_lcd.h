/*
 * lcd.h
 *
 *  Created on: Jan 21, 2026
 *      Author: ajy97
 */

#ifndef SRC_HARDWARE_APP_LCD_H_
#define SRC_HARDWARE_APP_LCD_H_
#include "bsp_lcd.h"

void lcd_init(void);
void lcd_send_string(char *str);
void lcd_put_cursor_buf(int row, int col);
void lcd_send_string_buf(char *str);

#endif /* SRC_HARDWARE_APP_LCD_H_ */
