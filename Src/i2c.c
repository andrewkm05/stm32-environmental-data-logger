/**
 * @file	i2c.c
 * @brief	Bare metal I2C1 driver for the STM32F446RE
 *
 * This file implements the initialization and communication routines of the STM32 I2C1 peripheral
 */

#include "i2c.h"
#include "stm32f4xx.h"

#define I2C_SCL_HZ	100000U
#define I2C_TIMEOUT	100000U

/**
 * @brief	Initializes the I2C peripheral in standard mode
 *
 * Configures the PB8 and PB9 for I2C operation and calculates the peripheral timing parameters based on PCLK1
 *
 * @param	pclk1_hz APB1 clock frequency in Hz
 *
 * @note External pull-up resistors are required on SDA and SCL
 * @note Supported PCLK1 frequencies are integer multiples of 1 MHz between 2 MHz and 45 MHz
 */

void I2C_init(uint32_t pclk1_hz)
{
	// Validate APB1 clock frequency
	if (pclk1_hz < 2000000U || pclk1_hz>45000000U || pclk1_hz % 1000000 != 0)
	{
		return;
	}

	//---------------------------------------------------------------------------

	/* 1. GPIO Configuration */

	// Enable GPIOB peripheral clock
	RCC->AHB1ENR |= (1U<<1);
	(void)RCC->AHB1ENR;

	// Configure both pins as open-drain outputs
	GPIOB->OTYPER |= (1U<<8) | (1U<<9);

	// Configure both pins for low output speed
	GPIOB->OSPEEDR &= ~((3U<<16) | (3U<<18));

	// Disable internal pull-up/pull-down resistors
	GPIOB->PUPDR &= ~((3U<<16) | (3U<<18));

	// Select AF4 (I2C1) as alternate function type for both pins
	GPIOB->AFR[1] &= ~((0xFU<<0) | (0xFU<<4));
	GPIOB->AFR[1] |= ((4U<<0) | (4U<<4));

	// Configure PB8 and PB9 in alternate function mode
	GPIOB->MODER &= ~((3U<<16) | (3U<<18));
	GPIOB->MODER |= ((2U<<16) | (2U<<18));

	//---------------------------------------------------------------------------

	/* 2. I2C1 Peripheral Configuration */

	// Enable I2C1 peripheral clock
	RCC->APB1ENR |= (1U<<21);
	(void)RCC->APB1ENR;

	// Disabling I2C1 before configuring timing registers
	I2C1->CR1 &= ~(1U<<0);

	// Convert APB1 clock frequency from Hz to MHz
	uint32_t freq = pclk1_hz / 1000000U;

	// Configure peripheral clock frequency
	I2C1->CR2 &= ~0x3FU;
	I2C1->CR2 |= freq;

	// Calculate clock control value for standard mode
	uint32_t ccr = pclk1_hz / (2U * I2C_SCL_HZ);

	// Select standard mode
	I2C1->CCR &= ~(1U<<15);

	// Configure clock control frequency
	I2C1->CCR &= ~0xFFFU;
	I2C1->CCR |= ccr;

	// Calculate maximum rise time value for standard mode
	uint32_t trise = freq + 1U;

	// Configure maximum rise time
	I2C1->TRISE &= ~0x3FU;
	I2C1->TRISE |= trise;

	// Enable I2C1 peripheral
	I2C1->CR1 |= (1U<<0);
}


/**
 * @brief	Checks the I2C peripheral for communication errors
 * @return	Returns error status or I2C_OK
 */
static I2C_Status I2C_check_errors(void)
{
	// Capture the current status register
	uint32_t sr1 = I2C1->SR1;

	// Check for arbitration loss
	if(sr1 & (1U<<9))
	{
		return I2C_ARBITRATION_ERROR;
	}

	// Check for bus error
	if(sr1 & (1U<<8))
	{
		return I2C_BUS_ERROR;
	}

	// Check for  failure
	if(sr1 & (1U<<10))
	{
		return I2C_NACK;
	}


	return I2C_OK;

}


/**
 * @brief	Wait until I2C bus becomes available
 * @return	Status of the bus availability check
 */
static I2C_Status I2C_wait_bus_free(void)
{

	// Initialize timeout counter
	uint32_t timeout = I2C_TIMEOUT;

	// Wait until bus is released
	while(I2C1->SR2 & (1U<<1))
	{
		if(timeout == 0U){
			return I2C_TIMEOUT_ERROR;
		}
		timeout--;
	}

	return I2C_OK;
}


/**
 * @brief	Generates a START or Repeated START condition
 * @return	Status of the START operation
 */
static I2C_Status I2C_start(void)
{
	// Initialize timeout counter
	uint32_t timeout = I2C_TIMEOUT;

	// Request START generation
	I2C1->CR1 |= (1U<<8);

	// Wait until the START condition is generated
	while(1)
	{
		// Check status
		I2C_Status status = I2C_check_errors();

		if(status != I2C_OK)
		{
			return status;
		}

		// Check whether START has been generated
		if(I2C1->SR1 & (1U<<0))
		{
			return I2C_OK;
		}

		// Check for Timeout
		if(timeout == 0U)
		{
			return I2C_TIMEOUT_ERROR;
		}

		timeout--;

	}
}


/**
 * @brief	Request the generation of a STOP condition
 */
static void I2C_stop()
{
	// Request a STOP generation
	I2C1->CR1 |= (1U<<9);
}

































