#include "crc.h"

static const uint8_t crc8_table[] = {
    0x00, 0x07, 0x0e, 0x09, 0x1c, 0x1b, 0x12, 0x15,
    0x38, 0x3f, 0x36, 0x31, 0x24, 0x23, 0x2a, 0x2d
};

uint8_t crc8(uint8_t *data, int len) {
    uint8_t crc = 0;

    while (len--) {
        crc = crc8_table[((crc >> 4u) ^ (*data >> 4u)) & 0xfu] ^ (crc << 4u);
        crc = crc8_table[((crc >> 4u) ^ *data) & 0xfu] ^ (crc << 4u);
        data++;
    }
    
    return crc;
}
