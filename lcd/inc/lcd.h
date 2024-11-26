/* lcd - lcd interface driver
 * Copyright (C) 2024 Olive Hudson <whudson@uwaterloo.ca>
 * see LICENCE file for licensing information */

#ifndef LCD_H
#define LCD_H

#include "stm32f4xx_hal.h"

typedef struct lcd {
	GPIO_TypeDef *gpio_etc, *gpio_data;
	uint32_t pin_select, pin_enable, pin1, pin2, pin3, pin4;
} lcd_t;

void lcd_reset(lcd_t *lcd);
void lcd_str(lcd_t *lcd, const char *line1, const char *line2);

#endif /* LCD_H */
