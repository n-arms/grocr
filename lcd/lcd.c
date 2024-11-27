/* lcd - lcd interface driver
 * Copyright (C) 2024 Olive Hudson <whudson@uwaterloo.ca>
 * see LICENCE file for licensing information */

#include <stdint.h>

#include "lcd.h"

#ifdef LCD_TEST
int HAL_GPIO_WritePin(int *a, int b, int c);
int HAL_Delay(int a);
#define GPIO_PIN_SET 0
#define GPIO_PIN_RESET 0
#endif /* LCD_TEST */

/* a select bit must be sent with the data to indicate to the lcd whether a
 * command or data is being sent */
typedef enum select {
	COMMAND = 0,
	DATA = 1
} select_t;

static void lcd_write(lcd_t *lcd, select_t select, uint8_t data);
static void lcd_enable(lcd_t *lcd);
static void lcd_send(lcd_t *lcd, select_t select, uint8_t cmd);

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
 * for at least 450ns, but is held for 1ms by the delays */
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
					 only guaranteed 1ms */
	lcd_send(lcd, COMMAND, 0x06); /* shift right after char input */
	lcd_send(lcd, COMMAND, 0x0C); /* display on, cursor off */
}

/* clear and return to the first line */
void
lcd_clear(lcd_t *lcd)
{
	lcd_send(lcd, COMMAND, 0x1); /* clear and return to first line */
	HAL_Delay(1);                /* above command requires 1.37ms delay,
					only guaranteed 1ms */
}

/* move the cursor to an index on the first line */
void
lcd_cur1(lcd_t *lcd, uint8_t cur)
{
	lcd_send(lcd, COMMAND, (cur & 0x4F) | 0x80);
}

/* move the cursor to an index on the second line */
void
lcd_cur2(lcd_t *lcd, uint8_t cur)
{
	lcd_send(lcd, COMMAND, (cur & 0x4F) | 0xC0);
}

/* write data to the lcd at the starting at the cursor index */
void
lcd_str(lcd_t *lcd, const char *str)
{
	for (uint8_t i = 0; str[i] != '\0'; ++i)
		lcd_send(lcd, DATA, str[i]);
}
