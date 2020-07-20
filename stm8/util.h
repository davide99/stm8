#ifndef STM8_UTIL_H
#define STM8_UTIL_H

#include "common.h"
#include <stdint.h>

#define UID_PTR ((volatile uint8_t *)(0x4865u))
#define UID_LEN 12u

void delay(uint16_t);
void uid_read(uint8_t *data);

#endif