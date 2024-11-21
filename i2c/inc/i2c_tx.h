#ifndef __I2C_H
#define __I2C_H

#include "stm32f4xx_hal.h"
#include "data_packet.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef struct I2C_tx_config {
	// GPIO config
	GPIO_TypeDef *clock_gpio;
	uint32_t clock_pin;
	GPIO_TypeDef *data_gpio;
	uint32_t data_pin;

	// driver config
	uint64_t millis_per_tick;
} I2C_tx_config;

typedef enum driver_state {
	CLOCK_END, DATA_ON_CLOCK_LOW, DATA_ON_CLOCK_HIGH
} driver_state;

// OWNERSHIP: `clock_gpio` and `data_gpio` are borrowed by I2C_driver and must be valid for its lifetime
// OWNERSHIP: `current.data` is owned by I2C_driver and will be destroyed in free_I2C_driver
typedef struct I2C_tx_driver {
	// driver state
	uint64_t next_tick;
	bool sending_data;
	driver_state state;
	data_packet current;

	I2C_tx_config config;
} I2C_tx_driver;

I2C_tx_driver new_I2C_tx_driver(I2C_tx_config config, size_t packet_length);

void free_I2C_driver(I2C_tx_driver *driver);

void tick_I2C_tx_driver(I2C_tx_driver *driver);

// SAFETY: data must be an array of at least `(driver -> current).length` elements
// OWNERSHIP: data is not borrowed by the I2C_driver and may be deallocated or overwritten as soon as `send_packet_I2C_driver` returns
void send_packet_I2C_driver(I2C_tx_driver *driver, bool data[]);

#endif
