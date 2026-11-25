/**
 * @file    i2c.h
 * @brief   Public interface for the STM32F446RE I2C1 driver.
 *
 * This header declares the functions used to initialize the I2C1 peripheral and communicate with external I2C devices.
 *
 * The interface allows other modules (e.g: the BMP280 sensor driver) to use I2C communication
 * without directly accessing the hardware registers.
 */

#ifndef I2C_H_
#define I2C_H_

#include <stdint.h>

/**
 * @brief	Status codes returned by I2C operations
 */
typedef enum
{
	I2C_OK = 0,
	I2C_TIMEOUT_ERROR,
	I2C_BUS_ERROR,
	I2C_ARBITRATION_ERROR,
	I2C_NACK
}I2C_Status;


void I2C_init(uint32_t pclk1_hz);

#endif /* I2C_H_ */
