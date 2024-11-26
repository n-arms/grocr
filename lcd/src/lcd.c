/* lcd - lcd interface driver
 * Copyright (C) 2024 Olive Hudson <whudson@uwaterloo.ca>
 * see LICENCE file for licensing information */

#include <stdint.h>
#include <string.h>

#include "lcd.h"

typedef enum select {
	COMMAND = 0,
	DATA = 1
} select_t;

static void lcd_write(lcd_t *lcd, select_t select, uint8_t data);
static void lcd_enable(lcd_t *lcd);
static void lcd_send(lcd_t *lcd, uint8_t cmd, select_t select);

/* lcd_write writes out given values to the gpio pins.  the data pins must
 * correspond to the lcd pins as follows:
 * data1 --> D4
 * data2 --> D5
 * data3 --> D6
 * data4 --> D7 */
static void
lcd_write(lcd_t *lcd, select_t select, uint8_t data)
{
	HAL_GPIO_WritePin(lcd->gpio_etc, lcd->pin_select, select);
	HAL_GPIO_WritePin(lcd->gpio_data, lcd->pin1, data & 1);
	HAL_GPIO_WritePin(lcd->gpio_data, lcd->pin2, (data >> 1) & 1);
	HAL_GPIO_WritePin(lcd->gpio_data, lcd->pin3, (data >> 2) & 1);
	HAL_GPIO_WritePin(lcd->gpio_data, lcd->pin4, (data >> 3) & 1);
}

/* lcd_enable activates the enable pin on the lcd, which causes a command or
 * data to be sent on the falling edge of a pulse.  the enable pin must be held
 * for 450ns, but is held for 1ms by the delays */
static void
lcd_enable(lcd_t *lcd)
{
	HAL_GPIO_WritePin(lcd->gpio_etc, lcd->pin_enable, GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(lcd->gpio_etc, lcd->pin_enable, GPIO_PIN_RESET);
	HAL_Delay(1);
}

/* lcd_send is a helper function which sends an eight byte command to the lcd
 * which uses four bit mode, thus requiring two writes */
static void
lcd_send(lcd_t *lcd, select_t select, uint8_t cmd)
{
	lcd_write(lcd, select, (cmd & 0xF0) >> 4);
	lcd_enable(lcd);
	lcd_write(lcd, select, cmd & 0xF);
	lcd_enable(lcd);
}

/* lcd_reset follows the procedure found on page 46 of the HD44780U data sheet,
 * which specifies how to reset the lcd and enter four bit mode.  three 0x3
 * bytes are sent, followed by a 0x2 byte.  then the commands commented below
 * are run */
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

	lcd_send(lcd, COMMAND, 0x28); /* four bit input, two lines, 5x8 font */
	lcd_send(lcd, COMMAND, 0x08); /* display off */
	lcd_send(lcd, COMMAND, 0x01); /* clear and return to first line */
	HAL_Delay(1);                 /* above command requires 1.37ms delay,
                                       * only guaranteed 1ms */
	lcd_send(lcd, COMMAND, 0x06); /* shift right after char input */
	lcd_send(lcd, COMMAND, 0x0C); /* display on, cursor off */
}

/* lcd_string clears the screen then sends a series of up to 16 characters for
 * each line.  after writing the first line, a command is run to move the
 * write address to 0x40, which is 64 characters into the rom in the lcd and
 * the start of the second line
 */
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
