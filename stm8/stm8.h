/*
 * STM8 peripheral library main header
 * 
 * Davide Pisanò
 */

#ifndef __SDCC
    #error Plase use sdcc
#endif

#ifndef F_CPU
    #warning CPU frequency not set, choosing 2MHz
    #define F_CPU 2000000UL
#endif

//Memory mapped registers starts at:
#define __RMM(addr) (*(volatile uint8_t *)(0x5000 + (addr)))

#include <stdint.h>

void delay(uint16_t);

#include "stm8_gpio.h"
