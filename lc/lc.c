#include "lc.h"

bool hx711_data[27];
hx711_driver hx711;

void init_hx711_driver(hx711_config config) {
  hx711.config = config;
  hx711.data = hx711_data;
  hx711.current_index = 0;
  hx711.is_new = false;
}

bool poll_hx711_driver(hx711_driver *driver) {
  return driver -> is_new;
}

uint32_t get_hx711_driver(hx711_driver *driver) {
	driver -> is_new = false;
	uint32_t number = 0;

	for (int i = 0; i < 27; i++) {
		number = number * 2 + driver -> data[i];
	}

	return number;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == hx711.config.dout_pin) {
	  HAL_TIM_Base_Start_IT(hx711.config.timer);
	  hx711.is_new = false;
  }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	HAL_GPIO_TogglePin(hx711.config.pd_sck_gpio, hx711.config.pd_sck_pin);
	int tick = ++hx711.current_index;
	if (tick % 2 == 1) {
		hx711.data[tick / 2] = HAL_GPIO_ReadPin(hx711.config.dout_gpio, hx711.config.dout_pin) == GPIO_PIN_SET;
	}
	if (tick == 27*2) {
		hx711.current_index = 0;
		HAL_TIM_Base_Stop_IT(hx711.config.timer);
		bool localData[27];
		memcpy(localData, hx711.data, 27 * sizeof(bool));
		__NOP();
		int number = 0;

		for (int i = 0; i < 27; i++) {
			number = number * 2 + localData[i];
		}
		hx711.is_new = true;
		__NOP();
	}
}
