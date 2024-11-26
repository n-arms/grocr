/* lc - load cell driver
 * Copyright (C) 2024 Nick Armstrong
 * Copyright (C) 2024 Olive Hudson <whudson@uwaterloo.ca>
 * see LICENCE file for licensing information */

#ifndef LC_H
#define LC_H

#include <stdbool.h>
#include <stdint.h>

#ifndef LC_TEST
#include "stm32f4xx_hal.h"
#else /* LC_TEST */
typedef int GPIO_TypeDef;
typedef int TIM_HandleTypeDef;
#endif /* LC_TEST */

/* lc_init must be called before any data can be received.  the parameters are
 * as follows:
 * gpio_data: the gpio of the data pin
 * pin_data: the data pin
 * gpio_clk: the gpio of the clock pin
 * pin_clk: the clock pin
 * timer: the hardware timer which reacts to interrupts */
void lc_init(GPIO_TypeDef *gpio_data, uint32_t pin_data,
		GPIO_TypeDef *gpio_clk, uint32_t pin_clk,
		TIM_HandleTypeDef *timer);
/* lc_poll returns false if the data is not ready and true otherwise */
bool lc_poll(void);
/* lc_get returns the data, and should only be called after lc_poll returned
 * true */
uint32_t lc_get(void);

#endif /* LC_H */
