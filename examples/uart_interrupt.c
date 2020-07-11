#include "../stm8/uart.h"

void main(){
    uart1_init(9600, 1);

    while(1);
}

void uart_isr_rx_full(void) __interrupt(UART1_RX_FULL_ISR){
    uart1_write(uart1_read());
}