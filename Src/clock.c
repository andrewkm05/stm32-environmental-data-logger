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
