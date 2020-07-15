#include "../../stm8/uart.h"
#include "../../stm8/util.h" //delay
#include "../../stm8/gpio.h"
#include "mfrc522.h"
#include <stdio.h>

volatile int8_t flag;

int putchar(int c){
    uart1_write(c);
    return c;
}

void isr_port_d(void) __interrupt(PD_ISR){
    if(!(PD_IDR & SHIFTL8(4u)))
        flag = 1;
}

void main(){
    flag = 0;
    
    uart1_init(9600, 0);
    printf("uart initialized\n");

    mfrc522_init();
    printf("mfrc522 initialized\n");

    mfrc522_init_interrupt();
    printf("mfrc522 interrupt initialized\n");

    printf("version: %X\n", mfrc522_get_version());

    while(1){
        if(flag){
            printf("interrupt\n");
            mfrc522_interrupt_clear();
            flag = 0;
        }

        mfrc522_interrupt_reactivate_reception();
        delay(100);
    }
}