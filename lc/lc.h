#ifndef __LC_H
#define __LC_H

#include "stm32f4xx_hal.h"
#include <stdbool.h>

typedef struct hx711_config {
  GPIO_TypeDef *dout_gpio;
  uint32_t dout_pin;
  GPIO_TypeDef *pd_sck_gpio;
  uint32_t pd_sck_pin;
  TIM_HandleTypeDef *timer;
} hx711_config;

typedef struct hx711_driver {
  hx711_config config;
  bool * volatile data;
  volatile uint32_t current_index;
  volatile bool clock_high;
  volatile bool has_data;
} hx711_driver;

// should be the only instance of hx711_driver,
// as the interrupt magic relies on talking to this global singleton
extern hx711_driver hx711;
extern bool hx711_data[27];

void init_hx711_driver(hx711_config config);
bool poll_hx711_driver(hx711_driver *driver);
uint32_t get_hx711_driver(hx711_driver *driver);

#endif
