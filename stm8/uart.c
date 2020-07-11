#include "uart.h"

#ifndef F_CPU
#define F_CPU 2000000UL
#endif

inline void uart1_init(uint16_t baudrate, int rx_isr_en){
    uint16_t uart_div;
    __asm__("rim");

    //divide F_CPU by baudrate with "rounding"
    uart_div = (F_CPU + (baudrate>>1u))/baudrate;
    
    //reference manual ask to fill BRR2 first
    UART1_BRR2 = (uint8_t)(((uart_div>>8u) & 0xF0u) | (uart_div & 0x0Fu));
    UART1_BRR1 = (uint8_t)((uart_div>>4u) & 0xFFu);

    //Enable RX, TX and RX interrupt
    UART1_CR2 = (rx_isr_en ? UART1_CR2_RIEN : 0) | UART1_CR2_TEN | UART1_CR2_REN;
}

void uart1_write(uint8_t data){
    while(!(UART1_SR & UART1_SR_TXE));
    UART1_DR = data;
    while(!(UART1_SR & UART1_SR_TC));
}

uint8_t uart1_read(){
    while(!(UART1_SR & UART1_SR_RXNE));
    return UART1_DR;
}
