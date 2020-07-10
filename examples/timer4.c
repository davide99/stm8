#include "../stm8/stm8.h"

#define LED_PIN 3

void main(){
    
    PD_DDR |= 1<<LED_PIN;
    PD_CR1 |= 1<<LED_PIN;
    
    TIM4_PSCR = 0;
    TIM4_ARR = 0;
    TIM4_IER |= TIM4_IER_UIE; //Enable update interrupt
    TIM4_CR1 |= TIM4_CR1_CEN; //Enable tim4

    while(1);
}

void tim4_isr(void) __interrupt(TIM4_ISR) {
    PD_ODR ^= 1<<LED_PIN;           //Toggle pin
    TIM4_SR &= ~TIM4_SR_UIF;    //Interrupt served
}