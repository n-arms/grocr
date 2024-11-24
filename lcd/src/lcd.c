/* lcd - lcd interface driver
 * Copyright (C) 2024 Olive Hudson <whudson@uwaterloo.ca>
 * see LICENCE file for licensing information */

#include <stdint.h>
#include <string.h>

#include "lcd.h"

static void lcd_enable(lcd_t *lcd);
static void lcd_wait(lcd_t *lcd);
static void lcd_send(lcd_t *lcd, uint8_t cmd, select_t select);

static void
lcd_write(lcd_t *lcd)
{
	// WRITE(PIN0, lcd->select);
	HAL_GPIO_WritePin(lcd->lcd_misc_gpio, lcd->select_pin, lcd->select);
	// WRITE(PIN1, lcd->rw);
	HAL_GPIO_WritePin(lcd->lcd_misc_gpio, lcd->rw_pin, lcd->rw);
	// WRITE(PIN2, lcd->enable);
	HAL_GPIO_WritePin(lcd->lcd_misc_gpio, lcd->enable_pin, lcd->enable);

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
	lcd->enable = LCD_ENABLE;
	lcd_write(lcd);
	/* delay 450ns */
	lcd->enable = LCD_DISABLE;
	lcd_write(lcd);
}

/* sending commands and data may not be effective if the internal processor is
 * still busy with the last piece of information sent, therefore it is
 * necessary to check the busy flag and only send more data once it clears.
 * doing so has the added benefit of making commands finish quicker. */
static void
lcd_wait(lcd_t *lcd)
{
	lcd_t copy;
	memcpy(&copy, lcd, sizeof(lcd_t));

	lcd->rw = READ;
	lcd->select = COMMAND;
	lcd->data = 0x8;
	lcd_write(lcd);

	do
		lcd_enable(lcd);
	while ((lcd->data & 0x8) == 0x8);

	memcpy(lcd, &copy, sizeof(lcd_t));
}

/* this function implements the algorithm necessary to send a command or
 * write data using only four data pins rather than the typical eight. */
static void
lcd_send(lcd_t *lcd, uint8_t cmd, select_t select)
{
	lcd->rw = WRITE;
	lcd->select = select;
	lcd->data = (cmd & 0xF0) >> 4;
	lcd_write(lcd);
	lcd_enable(lcd);

	lcd_wait(lcd);

	lcd->data = (cmd & 0xF);
	lcd_write(lcd);
	lcd_enable(lcd);

	lcd_wait(lcd);
}

/* a reset sequence must be run each time the lcd is powered on to enter four
 * bit operation. */
void lcd_reset(lcd_t *lcd)
{
	lcd->rw = WRITE;
	lcd->select = COMMAND;
	lcd->data = 0x3;
	lcd_write(lcd);

	/* delay 40ms */
	lcd_enable(lcd);
	/* delay 4.1ms */
	lcd_enable(lcd);
	/* delay 100us */

	lcd_send(lcd, COMMAND, 0x2C); /* four bit input, two lines, 5x10 font */
	lcd_send(lcd, COMMAND, 0x8);  /* display off */
	lcd_send(lcd, COMMAND, 0x1);  /* clear and return to first line */
	lcd_send(lcd, COMMAND, 0x7);  /* shift right after char input */
	lcd_send(lcd, COMMAND, 0xC);  /* display on, cursor off */
}

void lcd_string(lcd_t *lcd, const char *line1, const char *line2)
{
	lcd_send(lcd, COMMAND, 0x1); /* clear and return to first line */
	/* both strings limited to sixteen characters */
	for (uint8_t i = 0; i < 16 && line1[i] != '\0'; ++i)
		lcd_send(lcd, DATA, line1[i]);
	lcd_send(lcd, COMMAND, 0xC0); /* jump to second line */
	for (uint8_t i = 0; i < 16 && line2[i] != '\0'; ++i)
		lcd_send(lcd, DATA, line2[i]);
}
