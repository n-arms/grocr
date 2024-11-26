#ifndef __I2C_CONFIG_H
#define __I2C_CONFIG_H

/* Configuration for an I2C transmitter driver. */
typedef struct I2C_tx_config
{
    // GPIO config
    GPIO_TypeDef *clock_gpio;
    uint32_t clock_pin;
    GPIO_TypeDef *data_gpio;
    uint32_t data_pin;

    // driver config
    uint64_t millis_per_tick;
} I2C_tx_config;

/* Configuration for an I2C receiver driver. */
typedef struct I2C_rx_config
{
    // GPIO config
    GPIO_TypeDef *clock_gpio;
    uint32_t clock_pin;
    GPIO_TypeDef *data_gpio;
    uint32_t data_pin;
} I2C_rx_config;

#endif
