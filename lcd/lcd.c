/* lcd - lcd interface driver
 * Copyright (C) 2024 Olive Hudson <whudson@uwaterloo.ca>
 * see LICENCE file for licensing information */

#include <stdint.h>
#include <string.h>

typedef enum enable {
	DISABLE = 0,
	ENABLE = 1
} enable_t;

typedef enum rw {
	WRITE = 0,
	READ = 1
} rw_t;

typedef enum select {
	COMMAND = 0,
	DATA = 1
} select_t;

typedef struct lcd {
	volatile rw_t rw : 1;
	volatile select_t select : 1;
	volatile uint8_t data : 4;
	volatile enable_t enable : 1;
} lcd_t;

static void
enable(lcd_t *lcd)
{
	lcd->enable = ENABLE;
	/* delate at least 450ns */
	lcd->enable = DISABLE;
}

static void
wait(lcd_t *lcd)
{
	lcd_t copy;
	memcpy(&copy, lcd, sizeof(lcd_t));

	lcd->rw = READ;
	lcd->select = COMMAND;
	lcd->data = 0x8;

	do
		enable(lcd);
	while ((lcd->data & 0x8) == 0x8);

	memcpy(lcd, &copy, sizeof(lcd_t));
}

static void
send(lcd_t *lcd, uint8_t cmd, select_t select)
{
	lcd->rw = WRITE;
	lcd->select = select;
	lcd->data = (cmd & 0xF0) >> 4;

	enable(lcd);
	wait(lcd);

	lcd->data = (cmd & 0xF);

	enable(lcd);
	wait(lcd);
}

void
reset(lcd_t *lcd)
{
	lcd->rw = WRITE;
	lcd->select = COMMAND;
	lcd->data = 0x3;

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
string(lcd_t *lcd, char *str)
{
	send(lcd, COMMAND, 0x1); /* clear and return to first line */
	for (int i = 0; str[i] != '\0'; ++i)
		if (str[i] != '\n')
			send(lcd, DATA, str[i]);
		else
			send(lcd, COMMAND, 0xC0); /* jump to second line */
}
