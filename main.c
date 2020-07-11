#include "stm8/gpio.h"
#include "stm8/util.h"

#define LED_PIN 3

void main(){
    
    PD_DDR |= SHIFTL8(LED_PIN);
    PD_CR1 |= SHIFTL8(LED_PIN);

    while (1) {
        PD_ODR ^= SHIFTL8(LED_PIN);
        delay(250);
    }
}