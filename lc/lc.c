/* lc - load cell driver
 * Copyright (C) 2024 Nick Armstrong
 * Copyright (C) 2024 Olive Hudson <whudson@uwaterloo.ca>
 * see LICENCE file for licensing information */

#include <stdbool.h>
#include <stdint.h>

#include "lc.h"

#ifndef LC_TEST
#include "stm32f4xx_hal.h"
#else /* LC_TEST */
#define HAL_TIM_Base_Start_IT(a)
#define HAL_TIM_Base_Stop_IT(a)
#define HAL_GPIO_TogglePin(a, b)
#define HAL_GPIO_ReadPin(a, b) 1
#endif

typedef struct lc {
        GPIO_TypeDef *gpio_data, *gpio_clk;
        uint32_t pin_data, pin_clk;
        TIM_HandleTypeDef *timer;
        volatile uint32_t data, index;
        bool new;
} lc_t;

lc_t lc;

void
lc_init(GPIO_TypeDef *gpio_data, uint32_t pin_data,
		GPIO_TypeDef *gpio_clk, uint32_t pin_clk,
		TIM_HandleTypeDef *timer)
{
	lc.gpio_data = gpio_data;
	lc.pin_data = pin_data;
	lc.gpio_clk = gpio_clk;
	lc.pin_clk = pin_clk;
	lc.timer = timer;
	lc.data = lc.index = lc.new = 0;
}

bool
lc_poll(void)
{
	return lc.new;
}

uint32_t
lc_get(void)
{
	lc.new = false;
	return lc.data;
}

void
HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin != lc.pin_data)
		return;
	HAL_TIM_Base_Start_IT(lc.timer);
	if (lc.new)
		lc.data = 0;
	lc.new = false;
}

void
HAL_TIM_Period_ElapsedCallback(TIM_HandleTypeDef *htim)
{
	HAL_GPIO_TogglePin(lc.gpio_clk, lc.pin_clk);
	if (++lc.index & 1)
		lc.data |= HAL_GPIO_ReadPin(lc.gpio_data, lc.pin_data)
				<< (lc.index / 2);

	if (lc.index == 27 * 2) {
		HAL_TIM_Base_Stop_IT(lc.timer);
		lc.index = 0;
		lc.new = true;
	}
}
