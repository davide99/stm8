#ifndef STM8_UART_H
#define STM8_UART_H

#include "common.h"

//UART1 d5->tx, d6->rx
#define UART1_SR   __RMM(0x230)
#define UART1_DR   __RMM(0x231)
#define UART1_BRR1 __RMM(0x232)
#define UART1_BRR2 __RMM(0x233)
#define UART1_CR2  __RMM(0x235)

//UART1 bits
#define UART1_CR2_RIEN SHIFTL8(5)
#define UART1_CR2_TEN  SHIFTL8(3)
#define UART1_CR2_REN  SHIFTL8(2)
#define UART1_SR_TXE   SHIFTL8(7)
#define UART1_SR_TC    SHIFTL8(6)
#define UART1_SR_RXNE  SHIFTL8(5)

//ISR number
#define UART1_RX_FULL_ISR 18


void    uart1_init(uint16_t baudrate, int8_t rx_isr_en);
void    uart1_write(uint8_t data);
uint8_t uart1_read();
int     putchar(int c);

#endif