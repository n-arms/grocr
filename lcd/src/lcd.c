/* lcd - lcd interface driver
 * Copyright (C) 2024 Olive Hudson <whudson@uwaterloo.ca>
 * see LICENCE file for licensing information */

#include <stdint.h>
#include <string.h>

#include "lcd.h"

static void lcd_enable(lcd_t *lcd);
static void lcd_send(lcd_t *lcd, uint8_t cmd, select_t select);

static void
lcd_write(lcd_t *lcd)
{
	// WRITE(PIN0, lcd->select);
	HAL_GPIO_WritePin(lcd->lcd_misc_gpio, lcd->select_pin, lcd->select);

	// WRITE(PIN4, lcd->data & 0x1);
	HAL_GPIO_WritePin(lcd->lcd_data_gpio, lcd->data_pin_1, lcd->data & 0x1);
	// WRITE(PIN5, (lcd->data & 0x2) >> 1);
	HAL_GPIO_WritePin(lcd->lcd_data_gpio, lcd->data_pin_2, (lcd->data & 0x2) >> 1);
	// WRITE(PIN6, (lcd->data & 0x4) >> 2);
	HAL_GPIO_WritePin(lcd->lcd_data_gpio, lcd->data_pin_3, (lcd->data & 0x4) >> 2);
	// WRITE(PIN7, (lcd->data & 0x8) >> 3);
	HAL_GPIO_WritePin(lcd->lcd_data_gpio, lcd->data_pin_4, (lcd->data & 0x8) >> 3);
}

/* commands and data are indicated to be sent on a low pulse on the enable
 * pin */
static void
lcd_enable(lcd_t *lcd)
{
	HAL_GPIO_WritePin(lcd->lcd_misc_gpio, lcd->enable_pin, GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(lcd->lcd_misc_gpio, lcd->enable_pin, GPIO_PIN_RESET);
	HAL_Delay(1);
}

/* this function implements the algorithm necessary to send a command or
 * write data using only four data pins rather than the typical eight. */
static void
lcd_send(lcd_t *lcd, select_t select, uint8_t cmd)
{
	lcd->select = select;
	lcd->data = (cmd & 0xF0) >> 4;
	lcd_write(lcd);
	lcd_enable(lcd);

	lcd->data = (cmd & 0x0F);
	lcd_write(lcd);
	lcd_enable(lcd);
}

/* a reset sequence must be run each time the lcd is powered on to enter four
 * bit operation. */
void lcd_reset(lcd_t *lcd)
{
	lcd->select = COMMAND;
	lcd->data = 0x3;
	lcd_write(lcd);

	HAL_Delay(15);
	lcd_enable(lcd);
	HAL_Delay(5);
	lcd_enable(lcd);
	HAL_Delay(1);
	lcd_enable(lcd);

	lcd->data = 0x2;
	lcd_write(lcd);
	lcd_enable(lcd);

	lcd_send(lcd, COMMAND, 0x28); /* four bit input, two lines, 5x10 font */
	lcd_send(lcd, COMMAND, 0x08); /* display off */
	lcd_send(lcd, COMMAND, 0x01); /* clear and return to first line */
	HAL_Delay(5);
	lcd_send(lcd, COMMAND, 0x06); /* shift right after char input */
	lcd_send(lcd, COMMAND, 0x0C); /* display on, cursor on */
}

void lcd_string(lcd_t *lcd, const char *line1, const char *line2)
{
	lcd_send(lcd, COMMAND, 0x01); /* clear and return to first line */
	/* both strings limited to sixteen characters */
	for (uint8_t i = 0; i < 16 && line1[i] != '\0'; ++i)
		lcd_send(lcd, DATA, line1[i]);
	lcd_send(lcd, COMMAND, 0xC0); /* jump to second line */
	for (uint8_t i = 0; i < 16 && line2[i] != '\0'; ++i)
		lcd_send(lcd, DATA, line2[i]);
}
