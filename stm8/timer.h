#ifndef STM8_TIMER_H
#define STM8_TIMER_H

#include "common.h"

//Timer 4
//Registers
#define TIM4_CR1  __RMM(0x340)
#define TIM4_IER  __RMM(0x343)
#define TIM4_SR   __RMM(0x344)
#define TIM4_EGR  __RMM(0x345)
#define TIM4_CNTR __RMM(0x346)
#define TIM4_PSCR __RMM(0x347)
#define TIM4_ARR  __RMM(0x348)

//Registers' bits
#define TIM4_CR1_CEN SHIFTL8(0)
#define TIM4_IER_UIE SHIFTL8(0)
#define TIM4_SR_UIF  SHIFTL8(0)

//ISR number
#define TIM4_ISR  23

#endif