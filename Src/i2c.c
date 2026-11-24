/*
 * @file	i2c.h
 * @brief	Bare metal I2C1 driver for the STM32F446RE
 *
 * This file implements the initialization and communication routines of the STM32 I2C1 peripheral
 */

#include "i2c.h"
#include "stm32f4xx.h"

/*
 * @FUNCTION	I2C_init
 * @brief		Initializes the I2C peripheral
 *
 * Configures the PA8 and PA9 for I2C operation and initializes the peripheral to standar-mode communication
 */

void I2C_init(void)
{
	/* 1. GPIO Configuration */

	// Enable GPIOB peripheral clock
	RCC->AHB1ENR |= (1U<<1);

	// Configure PA8 and PA9 in alternate function mode
	GPIOB->MODER &= ~((3U<<16) | (3U<<18));
	GPIOB->MODER |= ((2U<<16) | (2U<<18));

	// Configure both pins as open-drain outputs
	GPIOB->OTYPER |= (1U<<8) | (1U<<9);

	// Configure both pins for low output speed
	GPIOB->OSPEEDR &= ~((3U<<16) | (3U<<18));

	// Disable internal pull-up/pull-down resistors
	GPIOB->PUPDR &= ~((3U<<16) | (3U<<18));

	// Select AF4 (I2C1) as alternate function type for both pins
	GPIOB->AFR[1] &= ~((0xFU<<0) | (0xFU<<4));
	GPIOB->AFR[1] |= ((4U<<0) | (4U<<4));


	/* I2C1 Peripheral Configuration */

	//



}
