#include "../stm8/gpio.h"
#include "../stm8/timer.h"
#include "../stm8/common.h"

#define LED_PIN 3

//Connect a led on pin D3, it should blink really quick (~30 Hz)

void main(){
    
    PD_DDR |= SHIFTL8(LED_PIN);
    PD_CR1 |= SHIFTL8(LED_PIN);
    
    TIM4_PSCR = 0b111;
    TIM4_ARR = 0xFF;
    TIM4_IER |= TIM4_IER_UIE; //Enable update interrupt
    TIM4_CR1 |= TIM4_CR1_CEN; //Enable tim4
    __asm__("rim");

    while(1);
}

void tim4_isr(void) __interrupt(TIM4_ISR) {
    PD_ODR ^= SHIFTL8(LED_PIN); //Toggle pin
    TIM4_SR &= ~TIM4_SR_UIF;    //Interrupt served
}