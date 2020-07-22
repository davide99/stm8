#ifndef STM8_COMMON_H
#define STM8_COMMON_H

#include <stdint.h>

//Memory mapped registers starts at:
#define __RMM(addr) (*(volatile uint8_t *)(0x5000u + (uint16_t)(addr)))
#define SHIFTL8(shamt) ((uint8_t)(1u<<((uint8_t)shamt)))
#define NEG8(val) ((uint8_t)(~(uint8_t)val))

#endif