#include "../stm8/gpio.h"

void EXTI_PORTD_IRQHandler(void) __interrupt(PD_ISR) {
    PD_ODR ^= SHIFTL8(3);     //  Toggle Port D, pin 3.
}

void main() {
    PD_DDR = SHIFTL8(3);          //  All pins are outputs.
    PD_CR1 = SHIFTL8(3);          //  Push-Pull outputs.
    PD_CR2 = SHIFTL8(4);          //  Output speeds up to 10 MHz.
    //
    //  Now configure the input pin.
    //
    //PD_DDR &= ~SHIFTL8(4);        //  PD4 is input.
    //PD_CR1 &= ~SHIFTL8(4);         //  PD4 is floating input.
    //
    //  Set up the interrupt.
    //
    EXTI_CR1 = 0b10000000;      //  Interrupt on falling edge.
    EXTI_CR2 &= 0b11111011;      //  Falling edge only.
    __asm__("rim");

    while (1);
}
