#ifndef __LC_H
#define __LC_H

#include "stm32f4xx_hal.h"
#include <stdbool.h>

typedef struct hx711_config {
  GPIO_TypeDef *dout_gpio;
  uint32_t dout_pin;
  GPIO_TypeDef *pd_sck_gpio;
  uint32_t pd_sck_pin;
};

typedef struct hx711_driver {
  hx711_config config;
} hx711_driver;

hx711_driver new_hx711_driver(hx711_config config);
void tick_hx711_driver(hx711_driver *driver);
bool poll_hx711_driver(hx711_driver *driver);
uint32_t get_hx711_driver(hx711_driver *driver);

#endif
