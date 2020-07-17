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
    Uid uid;
    uid.size = 1;
    
    uart1_init(9600, 0);
    printf("uart initialized\n");

    PCD_Init();
    printf("mfrc522 initialized\n");

    PCD_InitInterrupt();
    printf("mfrc522 interrupt initialized\n");

    printf("version: %X\n", PCD_GetVersion());

    while(1){
        if(flag){
            printf("interrupt\n");
            uint8_t a = PICC_ReadCardSerial(&uid);

            printf(">%d, %d<\n", uid.size, a);

            for(int i = 0; i<uid.size; i++)
                printf("%x ", uid.uidByte[i]);

            printf("\n");

            PCD_ClearInterrupt();
            PICC_HaltA();
            flag = 0;
        }

        PCD_InterruptReactivateReception();
        delay(100);
    }
}