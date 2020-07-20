#include "../stm8/uart.h"
#include "../stm8/util.h"
#include <stdio.h>

int putchar(int c){
    uart1_write(c);
    return c;
}

void main(){
    uint8_t i;
    uint8_t uid[UID_LEN];

    uid_read(uid);
    uart1_init(9600, 0);
    
    for(i=0; i<UID_LEN; i++){
        printf("%X ", uid[i]);
    }

    while(1);
}
