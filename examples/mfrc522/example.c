#include "mfrc522.h"
#include "../../stm8/uart.h"
#include "../../stm8/util.h"
#include "../../stm8/gpio.h"
#include <stdio.h>

int putchar(int c){
    uart1_write(c);
    return c;
}

void main(){
    
    uart1_init(9600, 0);
    printf("Uart inizializzata\n");

    mfrc522_init();
    //mfrc522_write_register(0x02u << 1u, 0xA0);
    printf("mfrc522 inizializzato\n");

    uint8_t val = mfrc522_read_register(0x37u << 1u);
    printf("%X\n", val);

    while(1){
        /*if (read){
            printf("int\n");

            mfrc522_write_register(0x04u << 1u, 0x7Fu);
            read = 0;
        }

        mfrc522_write_register(0x09u << 1u, 0x26u);
        mfrc522_write_register(0x01u << 1u, 0x0Cu);
        mfrc522_write_register(0x0Du << 1u, 0x87u);

        delay(10);*/
    }
}