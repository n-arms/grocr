/* lcd - lcd interface driver
 * Copyright (C) 2024 Olive Hudson <whudson@uwaterloo.ca>
 * see LICENCE file for licensing information */

#include "stm32f4xx_hal.h"

#ifndef LCD_H
#define LCD_H

#include "stm32f4xx_hal.h"

typedef enum enable
{
        LCD_DISABLE = 0,
        LCD_ENABLE = 1
} enable_t;

typedef enum rw
{
        WRITE = 0,
        READ = 1
} rw_t;

typedef enum select
{
        COMMAND = 0,
        DATA = 1
} select_t;

/* this structure serves as a placeholder for the io pins or addresses which
 * correspond to the labelled pins on the LCD as follows:
 * - rw -> RW
 * - select -> RS
 * - data -> DB7 DB6 DB5 DB4 (in this order)
 * - enable -> E */
typedef struct lcd
{
        GPIO_TypeDef *lcd_misc_gpio;
        uint32_t select_pin;
        uint32_t rw_pin;
        uint32_t enable_pin;
        GPIO_TypeDef *lcd_data_gpio;
        uint32_t data_pin_1;
        uint32_t data_pin_2;
        uint32_t data_pin_3;
        uint32_t data_pin_4;
        volatile rw_t rw : 1;
        volatile select_t select : 1;
        volatile uint8_t data : 4;
        volatile enable_t enable : 1;
	TIM_HandleTypeDef *timer;
} lcd_t;

/* resets the lcd after powering on */
void lcd_reset(lcd_t *lcd);
/* resets lines of text on lcd */
void lcd_string(lcd_t *lcd, const char *line1, const char *line2);

#endif /* LCD_H */
