#include "../stm8/gpio.h"
#include "../stm8/timer.h"
#include "../stm8/common.h"

#define LED_PIN 3

//Connect a led on pin D3, it should toggle every second

void main(){
    
    PD_DDR |= SHIFTL8(LED_PIN);
    PD_CR1 |= SHIFTL8(LED_PIN);
    
    TIM2_PSCR = 7;
    TIM2_ARRH = 0x3D;
    TIM2_ARRL = 0x08;
    TIM2_IER |= TIM2_IER_UIE; //Enable update interrupt
    TIM2_CR1 |= TIM2_CR1_CEN; //Enable tim2
    __asm__("rim");

    while(1);
}

void tim2_isr(void) __interrupt(TIM2_ISR) {
    PD_ODR ^= SHIFTL8(LED_PIN); //Toggle pin
    TIM2_SR1 &= ~TIM2_SR1_UIF;  //Interrupt served
}