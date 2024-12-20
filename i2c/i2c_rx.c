#include "i2c_rx.h"
#include "data_packet.h"

I2C_rx_driver new_I2C_rx_driver(I2C_rx_config config, bool *data, size_t packet_size, uint64_t message_dead_time) {
	data_packet packet;
	packet.data = data;
	packet.current_index = 0;
	packet.length = packet_size;

	I2C_rx_driver driver;
	driver.clock_high = false;
	driver.last_tick = HAL_GetTick();
	driver.packet = packet;
	driver.is_new = false;
	driver.config = config;
	driver.message_dead_time = message_dead_time;

	return driver;
}

void tick_I2C_rx_driver(I2C_rx_driver *driver) {
	bool clock = HAL_GPIO_ReadPin(driver -> config.clock_gpio, driver -> config.clock_pin) == GPIO_PIN_SET;

	if (clock != driver -> clock_high) {
		uint64_t elapsed = HAL_GetTick() - driver -> last_tick;

		if (clock) {
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
			if (driver -> packet.current_index != 0 && elapsed > driver -> message_dead_time) {
				driver -> packet.current_index = 0;
			}
			bool bit = HAL_GPIO_ReadPin(driver -> config.data_gpio, driver -> config.data_pin) == GPIO_PIN_SET;
			push_data_packet(&driver -> packet, bit);
			driver -> last_tick = HAL_GetTick();
			driver -> clock_high = true;
			driver -> is_new = data_packet_is_done(&driver -> packet);
		} else {
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);

			driver -> clock_high = false;
		}
	}
}

bool poll_I2C_driver(I2C_rx_driver *driver) {
	return driver -> is_new;
}

// only valid data if `poll_I2C_driver` returns true
bool *get_I2C_driver(I2C_rx_driver *driver) {
	return driver -> packet.data;
}

void reset_I2C_driver(I2C_rx_driver *driver) {
	driver -> is_new = false;
	driver -> packet.current_index = 0;
}
