#include "util.h"

#ifndef F_CPU
#define F_CPU 2000000UL
#endif

inline void delay(uint16_t ms){
    uint32_t i;

    for(i = 0; i<(F_CPU/18000UL)*ms; i++)
        __asm__("nop");
}