#include "util.h"

#ifndef F_CPU
#define F_CPU 2000000UL
#endif

inline void delay(uint16_t ms){
    uint32_t t = (F_CPU/11000UL)*ms;

    while(t)
        t--;
}