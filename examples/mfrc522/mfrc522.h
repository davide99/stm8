#ifndef MFRC522_H
#define MFRC522_H

#include <stdint.h>
#include "../../stm8/spi.h"

void mfrc522_init();
void mfrc522_write_register(uint8_t, uint8_t);
void mfrc522_init_interrupt();
uint8_t mfrc522_read_register(uint8_t);
uint8_t mfrc522_get_version();
void do_something();

#endif