/* lcd - lcd interface driver
 * Copyright (C) 2024 Olive Hudson <whudson@uwaterloo.ca>
 * see LICENCE file for licensing information */

#ifndef LCD_H
#define LCD_H

#ifndef LCD_TEST
#include "stm32f4xx_hal.h"
#else /* LCD_TEST */
typedef int GPIO_TypeDef;
#endif /* LCD_TEST */

/* lcd_t must be populated by gpio configurations and pin numbers */
typedef struct lcd {
	GPIO_TypeDef *gpio_etc, *gpio_data;
	uint32_t pin_select, pin_enable, pin1, pin2, pin3, pin4;
} lcd_t;

/* lcd_reset must be run before information can be written to the lcd */
void lcd_reset(lcd_t *lcd);
/* lcd_str writes to the two lines of the lcd display */
void lcd_str(lcd_t *lcd, const char *line1, const char *line2);

#endif /* LCD_H */
