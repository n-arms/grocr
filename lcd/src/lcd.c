/* lcd - lcd interface driver
 * Copyright (C) 2024 Olive Hudson <whudson@uwaterloo.ca>
 * see LICENCE file for licensing information */

#include <stdint.h>
#include <string.h>

#include "lcd.h"

typedef enum select {
	COMMAND = 0,
	DATA = 1
};

static void lcd_write(select_t select, uint8_t data);
static void lcd_enable(lcd_t *lcd);
static void lcd_send(lcd_t *lcd, uint8_t cmd, select_t select);

static void
lcd_write(lcd_t *lcd, select_t select, uint8_t data)
{
	HAL_GPIO_WritePin(lcd->lcd_misc_gpio, lcd->select_pin, select);
	HAL_GPIO_WritePin(lcd->gpio_data, lcd->data1, data & 1);
	HAL_GPIO_WritePin(lcd->gpio_data, lcd->data2, (data >> 1) & 1);
	HAL_GPIO_WritePin(lcd->gpio_data, lcd->data3, (data >> 2) & 1);
	HAL_GPIO_WritePin(lcd->gpio_data, lcd->data4, (data >> 3) & 1);
}

static void
lcd_enable(lcd_t *lcd)
{
	HAL_GPIO_WritePin(lcd->lcd_misc_gpio, lcd->enable_pin, GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(lcd->lcd_misc_gpio, lcd->enable_pin, GPIO_PIN_RESET);
	HAL_Delay(1);
}

static void
lcd_send(lcd_t *lcd, select_t select, uint8_t cmd)
{
	lcd_write(lcd, select, (cmd & 0xF0) >> 4);
	lcd_enable(lcd);
	lcd_write(lcd, selecct, cmd * 0xF);
	lcd_enable(lcd);
}

void
lcd_reset(lcd_t *lcd)
{
	lcd_write(lcd, COMMAND, 0x3);
	HAL_Delay(15);
	lcd_enable(lcd);
	HAL_Delay(5);
	lcd_enable(lcd);
	HAL_Delay(1);
	lcd_enable(lcd);
	lcd_write(lcd, COMMAND, 0x2);
	lcd_enable(lcd);

	lcd_send(lcd, COMMAND, 0x28); /* four bit input, two lines, 5x10 font */
	lcd_send(lcd, COMMAND, 0x08); /* display off */
	lcd_send(lcd, COMMAND, 0x01); /* clear and return to first line */
	HAL_Delay(5);
	lcd_send(lcd, COMMAND, 0x06); /* shift right after char input */
	lcd_send(lcd, COMMAND, 0x0C); /* display on, cursor on */
}

void
lcd_string(lcd_t *lcd, const char *line1, const char *line2)
{
	lcd_send(lcd, COMMAND, 0x01); /* clear and return to first line */
	/* both strings limited to sixteen characters */
	for (uint8_t i = 0; i < 16 && line1[i] != '\0'; ++i)
		lcd_send(lcd, DATA, line1[i]);
	lcd_send(lcd, COMMAND, 0xC0); /* jump to second line */
	for (uint8_t i = 0; i < 16 && line2[i] != '\0'; ++i)
		lcd_send(lcd, DATA, line2[i]);
}
