/* lcd - lcd interface driver
 * Copyright (C) 2024 Olive Hudson <whudson@uwaterloo.ca>
 * see LICENCE file for licensing information */

#include <stdint.h>
#include <string.h>

#include "lcd.h"

static void enable(lcd_t *lcd);
static void wait(lcd_t *lcd);
static void send(lcd_t *lcd, uint8_t cmd, select_t select);

static void
write(lcd_t *lcd)
{
	WRITE(PIN0, lcd->select);
	WRITE(PIN1, lcd->rw);
	WRITE(PIN2, lcd->enable);

	WRITE(PIN4, lcd->data & 0x1);
	WRITE(PIN5, (lcd->data & 0x2) >> 1);
	WRITE(PIN6, (lcd->data & 0x4) >> 2);
	WRITE(PIN7, (lcd->data & 0x8) >> 3);
}

/* commands and data are indicated to be sent on a low pulse on the enable
 * pin */
static void
enable(lcd_t *lcd)
{
	lcd->enable = ENABLE;
	write(lcd);
	/* delay 450ns */
	lcd->enable = DISABLE;
	write(lcd);
}

/* sending commands and data may not be effective if the internal processor is
 * still busy with the last piece of information sent, therefore it is
 * necessary to check the busy flag and only send more data once it clears.
 * doing so has the added benefit of making commands finish quicker. */
static void
wait(lcd_t *lcd)
{
	lcd_t copy;
	memcpy(&copy, lcd, sizeof(lcd_t));

	lcd->rw = READ;
	lcd->select = COMMAND;
	lcd->data = 0x8;
	write(lcd);

	do
		enable(lcd);
	while ((lcd->data & 0x8) == 0x8);

	memcpy(lcd, &copy, sizeof(lcd_t));
}

/* this function implements the algorithm necessary to send a command or
 * write data using only four data pins rather than the typical eight. */
static void
send(lcd_t *lcd, uint8_t cmd, select_t select)
{
	lcd->rw = WRITE;
	lcd->select = select;
	lcd->data = (cmd & 0xF0) >> 4;
	write(lcd);
	enable(lcd);

	wait(lcd);

	lcd->data = (cmd & 0xF);
	write(lcd);
	enable(lcd);

	wait(lcd);
}

/* a reset sequence must be run each time the lcd is powered on to enter four
 * bit operation. */
void
reset(lcd_t *lcd)
{
	lcd->rw = WRITE;
	lcd->select = COMMAND;
	lcd->data = 0x3;
	write(lcd);

	/* delay 40ms */
	enable(lcd);
	/* delay 4.1ms */
	enable(lcd);
	/* delay 100us */

	send(lcd, COMMAND, 0x2C); /* four bit input, two lines, 5x10 font */
	send(lcd, COMMAND, 0x8);  /* display off */
	send(lcd, COMMAND, 0x1);  /* clear and return to first line */
	send(lcd, COMMAND, 0x7);  /* shift right after char input */
	send(lcd, COMMAND, 0xC);  /* display on, cursor off */
}

void
string(lcd_t *lcd, const char *line1, const char *line2)
{
	send(lcd, COMMAND, 0x1); /* clear and return to first line */
	/* both strings limited to sixteen characters */
	for (uint8_t i = 0; i < 16 && str1[i] != '\0'; ++i)
		send(lcd, DATA, str1[i]);
	send(lcd, COMMAND, 0xC0); /* jump to second line */
	for (uint8_t i = 0; i < 16 && str2[i] != '\0'; ++i)
		send(lcd, DATA, str2[i]);
}
