#ifndef __I2C_H
#define __I2C_H

#include "stm32f4xx_hal.h"
#include "data_packet.h"
#include "i2c_config.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/* This module implements a driver for the transmission of I2C packets.

Example usage:

I2C_tx_config config;

I2C_tx_driver driver = new_I2C_tx_driver(config, 10);

while (1) {
	tick_I2C_tx_driver(&driver);

	bool message[10] = {0, 0, 0, 0, 0, 1, 1, 1, 1, 1};

	send_packet_I2C_driver(&driver, message);
}

free_I2C_driver(&driver);
*/

/* The internal state of the I2C driver. */
typedef enum driver_state
{
	CLOCK_END,
	DATA_ON_CLOCK_LOW,
	DATA_ON_CLOCK_HIGH
} driver_state;

/* The configuration and state of the I2C driver. */
// OWNERSHIP: `clock_gpio` and `data_gpio` are borrowed by I2C_driver and must be valid for its lifetime
// OWNERSHIP: `current.data` is owned by I2C_driver and will be destroyed in free_I2C_driver
typedef struct I2C_tx_driver
{
	// driver state
	uint64_t next_tick;
	bool sending_data;
	driver_state state;
	data_packet current;

	I2C_tx_config config;
} I2C_tx_driver;

/* Create a new I2C driver with a given config and packet size. */
I2C_tx_driver new_I2C_tx_driver(I2C_tx_config config, size_t packet_length);

/* Free the I2C driver. 
  Failure to call this function at the end of the driver's lifetime will result in a memory leak.*/
void free_I2C_driver(I2C_tx_driver *driver);

/* Update the state of I2C driver.
	This function should be called in a loop at least as frequently as the tick rate specified in the config. */
void tick_I2C_tx_driver(I2C_tx_driver *driver);

/* Send a packet of data through I2C. */
// SAFETY: data must be an array of at least `(driver -> current).length` elements
// OWNERSHIP: data is not borrowed by the I2C_driver and may be deallocated or overwritten as soon as `send_packet_I2C_driver` returns
void send_packet_I2C_driver(I2C_tx_driver *driver, bool data[]);

#endif
