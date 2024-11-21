#ifndef __I2C_H
#define __I2C_H

#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include <stdlib.h>

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
} I2C_config;

typedef struct I2C_driver {
	uint64_t last_tick;
	data_packet packet;
	bool is_new;
	bool clock_high;

	uint64_t message_dead_time;
	I2C_config config;
} I2C_driver;

void push_data_packet(data_packet *packet, bool value);
bool data_packet_is_done(data_packet *packet);

I2C_driver new_I2C_driver(I2C_config config, bool *data, size_t packet_size, uint64_t message_dead_time);
void tick_I2C_driver(I2C_driver *driver);
bool poll_I2C_driver(I2C_driver *driver);
// only valid data if `poll_I2C_driver` returns true
bool *get_I2C_driver(I2C_driver *driver);
void reset_I2C_driver(I2C_driver *driver);

#endif
