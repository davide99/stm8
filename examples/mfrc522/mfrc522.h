#ifndef MFRC522_H
#define MFRC522_H

#include <stdint.h>

#ifndef MFRC522_CS_PIN
    #define MFRC522_CS_PIN          4 //Port C
#endif

#ifndef MFRC522_IRQ_PIN
    #define MFRC522_IRQ_PIN         4 //Port D
#endif

void mfrc522_init();
void mfrc522_write_register(uint8_t, uint8_t);
void mfrc522_init_interrupt();
uint8_t mfrc522_read_register(uint8_t);
uint8_t mfrc522_get_version();
void mfrc522_interrupt_clear();
void mfrc522_interrupt_reactivate_reception();

#endif