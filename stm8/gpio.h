#ifndef STM8_GPIO_H
#define STM8_GPIO_H

#include "common.h"

//Port A
#define PA_ODR __RMM(0x00)
#define PA_IDR __RMM(0x01)
#define PA_DDR __RMM(0x02)
#define PA_CR1 __RMM(0x03)
#define PA_CR2 __RMM(0x04)

//Port B
#define PB_ODR __RMM(0x05)
#define PB_IDR __RMM(0x06)
#define PB_DDR __RMM(0x07)
#define PB_CR1 __RMM(0x08)
#define PB_CR2 __RMM(0x09)

//Port C
#define PC_ODR __RMM(0x0A)
#define PC_IDR __RMM(0x0B)
#define PC_DDR __RMM(0x0C)
#define PC_CR1 __RMM(0x0D)
#define PC_CR2 __RMM(0x0E)

//Port D
#define PD_ODR __RMM(0x0F)
#define PD_IDR __RMM(0x10)
#define PD_DDR __RMM(0x11)
#define PD_CR1 __RMM(0x12)
#define PD_CR2 __RMM(0x13)

//ISR
#define PA_ISR 3
#define PB_ISR 4
#define PC_ISR 5
#define PD_ISR 6

//Interrupt config registers
#define EXTI_CR1 __RMM(0xA0)
#define EXTI_CR2 __RMM(0xA1)

#endif