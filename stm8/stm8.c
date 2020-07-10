#include "stm8.h"

inline void delay(uint16_t ms){
    uint32_t i;

    //NOP takes always 1 cycle
    //ADDW (for i++) takes always 1 cycle
    for(i = 0; i<(F_CPU/18000UL)*ms; i++)
        __asm__("nop");
}