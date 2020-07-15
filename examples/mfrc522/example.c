#include "mfrc522.h"
#include "../../stm8/uart.h"
#include "../../stm8/util.h"
#include "../../stm8/gpio.h"
#include <stdio.h>

volatile int8_t flag;

int putchar(int c){
    uart1_write(c);
    return c;
}

void isrd(void) __interrupt(PD_ISR){
    if(!(PD_IDR & SHIFTL8(4u)))
        flag = 1;
}

void main(){
    flag = 0;
    
    uart1_init(9600, 0);
    printf("Uart inizializzata\n");

    mfrc522_init();
    printf("mfrc522 inizializzato\n");
    mfrc522_init_interrupt();
    printf("mfrc522 irq inizializzato\n");

    printf("%X\n", mfrc522_get_version());

    while(1){
        
        if(flag){
            printf("here\n");
            printf("interrupt\n");
            mfrc522_write_register(0x04u << 1u, 0x7Fu);
            flag = 0;
        }

        do_something();
        delay(100);
    }
}