#ifndef __I2C_H
#define __I2C_H

#include "stm32f4xx_hal.h"
#include "data_packet.h"
#include "i2c_config.h"
#include <stdlib.h>
#include <stdbool.h>

/* This module implements the driver for receiving I2C packets.

Example usage:

I2C_rx_config config;
I2C_rx_driver d = new_I2C_rx_driver(config, backing_array, 24, 500)

while (1) {
	tick_I2C_rx_driver(&d);
	if (poll_I2C_driver(&d)) {
		bool *data = get_I2C_driver(&d);

		// use data

		reset_I2C_driver(&d);
	}
}
*/

/* The state and configuration of the driver. */
typedef struct I2C_rx_driver
{
	uint64_t last_tick;
	data_packet packet;
	bool is_new;
	bool clock_high;

	uint64_t message_dead_time;
	I2C_rx_config config;
} I2C_rx_driver;

/* Create a new I2C driver. */
I2C_rx_driver new_I2C_rx_driver(I2C_rx_config config, bool *data, size_t packet_size, uint64_t message_dead_time);

/* Update the state of the I2C driver.
Should be called in a loop at least as frequently as the tick rate of the attached I2C_tx driver. */
void tick_I2C_rx_driver(I2C_rx_driver *driver);
/* Check if the driver has new data. */
bool poll_I2C_driver(I2C_rx_driver *driver);
/* Get the most recent packet received. Only valid if `poll_I2C_driver` returns true. */
bool *get_I2C_driver(I2C_rx_driver *driver);
/* Mark the most recently received packet as "stale", so that the driver is ready to a receive new packets. */
void reset_I2C_driver(I2C_rx_driver *driver);

#endif
