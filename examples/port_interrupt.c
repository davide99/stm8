#include "../stm8/gpio.h"

/*
 * +---------+
 * |       D3|------(LED)---GND
 * |    M  D4|------(BTN)---GND
 * |    C    |
 * |    U    |  NB: Since D4 is configured as pull-up,
 * |         |      normally is @ high. When the BTN
 * +---------+      is pressed D4 goes to low.
 */

//Both on port D
#define IN_PIN  4
#define OUT_PIN 3

void EXTI_PORTD_IRQHandler(void) __interrupt(PD_ISR) {
    /*
     * Since the interrupt is fired on falling edge, the
     * value on the pin should be low.
     * It isn't really mandatory here, since only IN_PIN
     * can trigger an interrupt.
     */

    if(!(PD_IDR & SHIFTL8(IN_PIN)))
        PD_ODR ^= SHIFTL8(OUT_PIN);     //Toggle OUT_PIN
}

void main() {
    PD_DDR |= SHIFTL8(OUT_PIN); //output
    PD_DDR &= ~SHIFTL8(IN_PIN); //input

    PD_CR1 |= SHIFTL8(IN_PIN) | SHIFTL8(OUT_PIN); //pull-up input, push-pull output
    PD_CR2 = SHIFTL8(IN_PIN);                     //enable interrupt on IN_PIN

    EXTI_CR1 |= 0b10000000; //<=+
    EXTI_CR1 &= 0b10111111; //<== interrupt on falling edge only
    EXTI_CR2 &= 0b11111011; //<== interrupt sensitivity on falling edge
    __asm__("rim");

    while (1){
        __asm__("wfi"); //wait for interrupt
    }
}
