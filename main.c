#include "stm8/stm8.h"

#define LED_PIN 3

void main(){
    
    PD_DDR |= 1<<LED_PIN;
    PD_CR1 |= 1<<LED_PIN;

    while (1) {
        PD_ODR ^= 1<<LED_PIN;
        delay(250);
    }
}