#include <stdint.h>

//Memory mapped registers starts at:
#define __RMM(addr) (*(volatile uint8_t *)(0x5000 + (addr)))
#define SHIFTL8(shamt) ((uint8_t)(1u<<((uint8_t)shamt)))