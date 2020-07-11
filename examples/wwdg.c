#include "../stm8/util.h"
#include "../stm8/wwdg.h"
#include "../stm8/uart.h"

void print(char* c){
    while(*c){
        uart1_write(*c);
        c++;
    }
}

void main(){
    uart1_init(9600, 0);

    //7:enable, 6:must be 1, [5:0]time
    WWDG_CR = 0xFF;
    
    /*
     * the mcu will be resetted if WR[6:0]<CR[6:0], or if
     * CR[6] == 0.
     * Default values: WR=CR=0x7F
     * 
     * Since CR[5:0]=0b111111, we have a window of
     * 12288*(CR[5:0]+1)/f_cpu = 0.4 s before the the watchdog
     * reset the mcu
     */

    print("starting\n");

    while(1) {
        print("in loop\n");

        /*
         * Try to remove the following line: the mcu will be
         * resetted and a "starting" message will be printed
         */

        WWDG_CR |= 0x7F;

        delay(10);
    }
}