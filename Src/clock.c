/**
 * @file	clock.c
 * @brief	System and peripheral clock calculations
 *
 * Implements functions for determining the current nominal SYSCLK, HCLK, PCLK1 and PCLK2 frequencies of STM32
 * by reading and decoding the RCC configuration registers
 *
 * (this module does not modify the existing clock configuration)
 */

#include "stm32f4xx.h"
#include "clock.h"

#define CLOCK_HSI_HZ    16000000U
#define CLOCK_HSE_HZ    8000000U

// AHB prescaler decoding table
static const uint16_t ahb_prescaler[16]=
{
		1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U,
		2U, 4U, 8U, 16U, 64U, 128U, 256U, 512U
};

// APB prescaler decoding table
static const uint8_t apb_prescaler[8]=
{
		1U, 1U, 1U, 1U, 2U, 4U, 8U, 16U
};


/**
 * @brief		Returns the current nominal system clock frequency
 * @return		System clock frequency in Hz, or 0 if undetermined
 */
uint32_t Clock_GetSYSCLK(void)
{
	// Read the active system clock source
	uint32_t sws = (RCC->CFGR >> 2) & 0x3U;

	switch(sws)
	{
	case 0U:
		// HSI
		return CLOCK_HSI_HZ;

	case 1U:
		// HSE
		return CLOCK_HSE_HZ;

	case 2U:
	{
		// PLL
		uint32_t pllcfgr= RCC->PLLCFGR;

		// Extract PLL configuration fields:
		uint32_t pllsrc = (pllcfgr >> 22) & 0x1U;
		uint32_t pllm = pllcfgr & 0x3FU;
		uint32_t plln = (pllcfgr >> 6) & 0x1FFU;
		uint32_t pllp = (pllcfgr >> 16) & 0x3U;

		// Decode the PLL output division factor
		pllp = 2U * (pllp + 1U);

		// Determine the PLL input clock frequency
		uint32_t pll_source_hz;

		if(pllsrc == 0U)
		{
			pll_source_hz = CLOCK_HSI_HZ;
		}
		else
		{
			pll_source_hz = CLOCK_HSE_HZ;
		}

		// Reject invalid PLL division or multiplication factors
		if(pllm < 2U || plln < 50U || plln > 432U)
		{
			return 0U;
		}

		// Calculate the nominal PLL system clock frequency
		return (uint32_t)(((uint64_t)pll_source_hz * plln) / (pllm * pllp));
	}

	default:
		// Invalid clock source
		return 0;
	}

}



/**
 * @brief	Returns the current nominal AHB clock frequency
 * @return	AHB clock frequency in Hz, or 0 if undetermined
 */
uint32_t Clock_GetHCLK(void)
{
	// Obtain the current system clock frequency
	uint32_t sysclk = Clock_GetSYSCLK();

	// Extract the AHB prescaler field
	uint32_t hpre = (RCC->CFGR >> 4) &  0xFU;

	// Decode the actual AHB division factor
	uint32_t divisior = ahb_prescaler[hpre];

	// Calculate the AHB clock frequency
	return (sysclk / divisior);
}


/**
 * @brief	Returns the current nominal APB1 or APB2 clock frequency
 * @return	APB1 or APB2 clock frequency in Hz, or 0 if undetermined
 */
uint32_t Clock_GetPCLK1(void)
{
	// Obtain the current AHB clock frequency
	uint32_t hclk = Clock_GetHCLK();

	// Extract the APB1 prescaler field
	uint32_t ppre1 = (RCC->CFGR >> 10) & 0xFU;

	// Decode the actual APB1 division factor
	uint32_t divisor1 = apb_prescaler[ppre1];

	// Calculate the APB1 clock frequency
	return (hclk / divisor1);
}


uint32_t Clock_GetPCLK2(void)
{
	// Obtain the current AHB clock frequency
	uint32_t hclk = Clock_GetHCLK();

	// Extract the APB1 prescaler field
	uint32_t ppre2 = (RCC->CFGR >> 13) & 0xFU;

	// Decode the actual APB1 division factor
	uint32_t divisor2 = apb_prescaler[ppre2];

	// Calculate the APB1 clock frequency
	return (hclk / divisor2);
}










