#include "lc.h"

bool hx711_data[27];
hx711_driver hx711;

void init_hx711_driver(hx711_config config) {
  hx711.config = config;
  hx711.data = hx711_data;
  hx711.current_index = 0;
  hx711.clock_high = false;
  hx711.has_data = false;
}
bool poll_hx711_driver(hx711_driver *driver) {
  return driver -> has_data;
}
uint32_t get_hx711_driver(hx711_driver *driver) {
  return 0;
}

void push_bit_hx711_driver(hx711_driver *driver, bool bit) {
  driver -> data[driver -> current_index++] = bit;
}

// NOTE: This function overloads a __weak function defined in the HAL.
// Don't change its name.
// remember to disable this interrupt while the transfer is happening
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
  // check that the interrupt was caused by the `dout` pin
  if (GPIO_Pin == hx711.config.dout_pin) {
    // block for at least 1 clock cycle (125 ns) to allow for the first clock cycle
    __NOP();

    hx711.has_data = false;
    // start the timer
    HAL_TIM_Base_Start(hx711.config.timer);
  }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
  if (hx711.clock_high) {
    // if the clock was high last tick, set the clock low, push data to buffer
    HAL_GPIO_WritePin(hx711.config.clock_gpio, hx711.config.clock_pin, GPIO_PIN_RESET);

    bool bit = (HAL_GPIO_ReadPin(hx711.config.dout_gpio, hx711.config.dout_pin) == GPIO_PIN_SET;
    push_bit_hx711_driver(&hx711, bit);
    
    hx711.clock_high = false;

    if (hx711.current_index == 27) {
      HAL_TIM_Base_Stop(hx711.config.timer);
      hx711.has_data = true;
    }
  } else {
    // if the clock was low last tick, set the clock high
    HAL_GPIO_WritePin(hx711.config.clock_gpio, hx711.config.clock_pin, GPIO_PIN_SET);

    hx711.clock_high = true;
  }
}
