#include "i2c.h"

// should only be called if data_packet_is_done returns false
bool pop_data_packet(data_packet *packet) {
	if (packet -> length == packet -> current_index) {
		return false;
	}
	return (packet -> data)[packet -> current_index++];
}

bool data_packet_is_done(data_packet *packet) {
	return packet -> length <= packet -> current_index;
}

void reset_data_packet(data_packet *packet, bool *new_data) {
	memcpy(packet -> data, new_data, packet -> length);
	packet -> current_index = 0;
}

I2C_driver new_I2C_driver(I2C_config config, size_t packet_length) {
	data_packet current = {
		.data = malloc(sizeof(bool) * packet_length),
		.length = packet_length,
		.current_index = 0
	};
	I2C_driver driver = {
		.next_tick = HAL_GetTick(),
		.sending_data = false,
		.clock_high = false,
		.current = current,
		.config = config
	};
	return driver;
}

void free_I2C_driver(I2C_driver *driver) {
	free((driver -> current).data);
}

void tick_I2C_driver(I2C_driver *driver) {
	if (driver -> sending_data && HAL_GetTick() > driver -> next_tick) {
		if (driver -> clock_high) {
			driver -> clock_high = false;
			driver -> next_tick += driver -> config.millis_per_tick;
			HAL_GPIO_WritePin(driver -> config.clock_gpio, driver -> config.clock_pin, GPIO_PIN_RESET);
		} else {
			driver -> clock_high = true;
			driver -> next_tick += driver -> config.millis_per_tick;
			bool bit = pop_data_packet(&driver -> current);
			HAL_GPIO_WritePin(driver -> config.data_gpio, driver -> config.data_pin, bit ? GPIO_PIN_SET : GPIO_PIN_RESET);
			HAL_GPIO_WritePin(driver -> config.clock_gpio, driver -> config.clock_pin, GPIO_PIN_SET);

			if (data_packet_is_done(&driver -> current)) {
				driver -> sending_data = false;
			}
		}
	}
}

// SAFETY: data must be an array of at least `(driver -> current).length` elements
// OWNERSHIP: data is not borrowed by the I2C_driver and may be deallocated or overwritten as soon as `send_packet_I2C_driver` returns
void send_packet_I2C_driver(I2C_driver *driver, bool data[]) {
	if (!driver -> sending_data) {
		reset_data_packet(&driver -> current, data);
		driver -> sending_data = true;
		driver -> next_tick = HAL_GetTick() + driver -> config.millis_per_tick;
	}
}
