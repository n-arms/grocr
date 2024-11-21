#ifndef __I2C_H
#define __I2C_H

#include "stm32f4xx_hal.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef struct data_packet {
	bool *data;
	size_t length;
	size_t current_index;
} data_packet;

typedef struct I2C_config {
	// GPIO config
	GPIO_TypeDef *clock_gpio;
	uint32_t clock_pin;
	GPIO_TypeDef *data_gpio;
	uint32_t data_pin;

	// driver config
	uint64_t millis_per_tick;
} I2C_config;

// OWNERSHIP: `clock_gpio` and `data_gpio` are borrowed by I2C_driver and must be valid for its lifetime
// OWNERSHIP: `current.data` is owned by I2C_driver and will be destroyed in free_I2C_driver
typedef struct I2C_driver {
	// driver state
	uint64_t next_tick;
	bool sending_data;
	bool clock_high;
	data_packet current;

	I2C_config config;
} I2C_driver;

bool pop_data_packet(data_packet *packet);

bool data_packet_is_done(data_packet *packet);

void reset_data_packet(data_packet *packet, bool *new_data);

I2C_driver new_I2C_driver(I2C_config config, size_t packet_length);

void free_I2C_driver(I2C_driver *driver);

void tick_I2C_driver(I2C_driver *driver);

// SAFETY: data must be an array of at least `(driver -> current).length` elements
// OWNERSHIP: data is not borrowed by the I2C_driver and may be deallocated or overwritten as soon as `send_packet_I2C_driver` returns
void send_packet_I2C_driver(I2C_driver *driver, bool data[]);

#endif
