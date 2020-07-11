#ifndef STM8_TIMER_H
#define STM8_TIMER_H

#include "common.h"

//Timer 2
//Registers
#define TIM2_CR1   __RMM(0x300)
#define TIM2_IER   __RMM(0x303)
#define TIM2_SR1   __RMM(0x304)
#define TIM2_SR2   __RMM(0x305)
#define TIM2_EGR   __RMM(0x306)
#define TIM2_CNTRH __RMM(0x30C)
#define TIM2_CNTRL __RMM(0x30D)
#define TIM2_PSCR  __RMM(0x30E)
#define TIM2_ARRH  __RMM(0x30F)
#define TIM2_ARRL  __RMM(0x310)

//Registers' bits
#define TIM2_CR1_CEN SHIFTL8(0)
#define TIM2_IER_UIE SHIFTL8(0)
#define TIM2_SR1_UIF SHIFTL8(0)

//ISR number
#define TIM2_ISR 13

//-----------------------------

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
#define TIM4_ISR 23

#endif