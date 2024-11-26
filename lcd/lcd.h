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
/* clear and return to the first line */
void lcd_clear(lcd_t *lcd);
/* move the cursor to an index on the first line */
void lcd_cur1(lcd_t *lcd, uint8_t cur);
/* move the cursor to an index on the second line */
void lcd_cur2(lcd_t *lcd, uint8_t cur);
/* write data to the lcd at the starting at the cursor index */
void lcd_str(lcd_t *lcd, const char *str);

#endif /* LCD_H */
