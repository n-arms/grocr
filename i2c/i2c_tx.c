#include "i2c_tx.h"
#include "data_packet.h"

I2C_tx_driver new_I2C_tx_driver(I2C_tx_config config, size_t packet_length) {
	data_packet current = {
		.data = malloc(sizeof(bool) * packet_length),
		.length = packet_length,
		.current_index = 0
	};
	I2C_tx_driver driver = {
		.next_tick = HAL_GetTick(),
		.sending_data = false,
		.state = CLOCK_END,
		.current = current,
		.config = config
	};
	return driver;
}

void free_I2C_driver(I2C_tx_driver *driver) {
	free((driver -> current).data);
}

void tick_I2C_tx_driver(I2C_tx_driver *driver) {
	if (driver -> sending_data && HAL_GetTick() > driver -> next_tick) {
		if (driver -> state == CLOCK_END) {
			driver -> state = DATA_ON_CLOCK_LOW;
			driver -> next_tick += driver -> config.millis_per_tick / 2;
			bool bit = pop_data_packet(&driver -> current);
			HAL_GPIO_WritePin(driver -> config.data_gpio, driver -> config.data_pin, bit ? GPIO_PIN_SET : GPIO_PIN_RESET);
		} else if (driver -> state == DATA_ON_CLOCK_LOW) {
			driver -> state = DATA_ON_CLOCK_HIGH;
			driver -> next_tick += driver -> config.millis_per_tick;
			HAL_GPIO_WritePin(driver -> config.clock_gpio, driver -> config.clock_pin, GPIO_PIN_SET);
		} else {
			driver -> state = CLOCK_END;
			driver -> next_tick += driver -> config.millis_per_tick / 2;
			HAL_GPIO_WritePin(driver -> config.clock_gpio, driver -> config.clock_pin, GPIO_PIN_RESET);

			if (data_packet_is_done(&driver -> current)) {
				driver -> sending_data = false;
			}
		}
	} else if(!driver -> sending_data){
		HAL_GPIO_WritePin(driver -> config.clock_gpio, driver -> config.clock_pin, GPIO_PIN_RESET);
	}
}

// SAFETY: data must be an array of at least `(driver -> current).length` elements
// OWNERSHIP: data is not borrowed by the I2C_driver and may be deallocated or overwritten as soon as `send_packet_I2C_driver` returns
void send_packet_I2C_driver(I2C_tx_driver *driver, bool data[]) {
	if (!driver -> sending_data) {
		reset_data_packet(&driver -> current, data);
		driver -> sending_data = true;
		driver -> next_tick = HAL_GetTick() + driver -> config.millis_per_tick;
	}
}
