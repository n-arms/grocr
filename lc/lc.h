#ifndef __LC_H
#define __LC_H

#include "stm32f4xx_hal.h"
#include <stdbool.h>
#include <string.h>

/*
This module implements a driver for communicating with the hx711 amplifier when connected to a load cell.

Example usage:

hx711_config config;
config.dout_gpio = GPIOA;
config.dout_pin = GPIO_PIN_0;
config.pd_sck_gpio = GPIOA;
config.pd_sck_pin = GPIO_PIN_1;
config.timer = &tim1;

init_hx711_driver(config);

while (1) {
  if (poll_hx711_driver(&hx711)) {
    uint32_t data = get_hx711_driver(&hx711);
  }
}
*/

/* The configuration for the hx711 driver. */
typedef struct hx711_config {
  GPIO_TypeDef *dout_gpio;
  uint32_t dout_pin;
  GPIO_TypeDef *pd_sck_gpio;
  uint32_t pd_sck_pin;
  TIM_HandleTypeDef *timer;
} hx711_config;

/* The driver, encapsulating internal state and tracking the config. */
typedef struct hx711_driver {
  hx711_config config;
  bool * volatile data;
  volatile uint32_t current_index;
  bool is_new;
} hx711_driver;

/* The singleton instance of the driver. Creating more drivers is undefined. */
extern hx711_driver hx711;
extern bool hx711_data[27];

/* Initialize the global `hx711` driver singleton using the given config. */
void init_hx711_driver(hx711_config config);

/* Check if the driver has received a new packet. */
bool poll_hx711_driver(hx711_driver *driver);

/* Get the value of the current packet, marking it as "stale"
  so that future calls to `poll_hx711_driver` return false. */
uint32_t get_hx711_driver(hx711_driver *driver);

#endif
