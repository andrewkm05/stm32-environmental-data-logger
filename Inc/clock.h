/*
 * clock.h
 *
 *  Created on: 24 Nov 2026
 *      Author: andreaskoumi
 */

#ifndef CLOCK_H_
#define CLOCK_H_

#include <stdint.h>

uint32_t Clock_GetSYSCLK(void);
uint32_t Clock_GetHCLK(void);
uint32_t Clock_GetPCLK1(void);
uint32_t Clock_GetPCLK2(void);


#endif /* CLOCK_H_ */
