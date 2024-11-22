/* lcd - lcd interface driver
 * Copyright (C) 2024 Olive Hudson <whudson@uwaterloo.ca>
 * see LICENCE file for licensing information */

#ifndef LCD_H
#define LCD_H

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

/* this structure serves as a placeholder for the io pins or addresses which
 * correspond to the labelled pins on the LCD as follows:
 * - rw -> RW
 * - select -> RS
 * - data -> DB7 DB6 DB5 DB4 (in this order)
 * - enable -> E */
typedef struct lcd {
        volatile rw_t rw : 1;
        volatile select_t select : 1;
        volatile uint8_t data : 4;
        volatile enable_t enable : 1;
} lcd_t;

/* resets the lcd after powering on */
void reset(lcd_t *lcd);
/* resets lines of text on lcd */
void string(lcd_t *lcd, const char *line1, const char *line2);

#endif /* LCD_H */
