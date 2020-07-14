#include "mfrc522.h"
#include "../../stm8/gpio.h"
#include "../../stm8/util.h"

#define CS_PIN              4

#define PCD_RESETPHASE      0x0F

#define MFRC522_REG_COMMAND	    0x01u << 1u
#define MFRC522_REG_MODE        0x11u << 1u
#define MFRC522_REG_TX_MODE	    0x12u << 1u
#define MFRC522_REG_RX_MODE	    0x13u << 1u
#define MFRC522_REG_TX_CONTROL  0x14u << 1u
#define MFRC522_REG_TX_AUTO     0x15u << 1u
#define MFRC522_REG_MOD_WIDTH   0x24u << 1u
#define MFRC522_REG_T_MODE      0x2Au << 1u
#define MFRC522_REG_T_PRESCALER	0x2Bu << 1u
#define MFRC522_REG_T_RELOAD_H  0x2Cu << 1u
#define MFRC522_REG_T_RELOAD_L  0x2Du << 1u
#define MFRC522_REG_RF_CFG      0x26u << 1u

void mfrc522_write_register(uint8_t reg, uint8_t val){
    spi_begin_transaction();
    PC_ODR &= ~SHIFTL8(CS_PIN); //low
    spi_transfer(reg);
    spi_transfer(val);
    PC_ODR |= SHIFTL8(CS_PIN); //high
    spi_end_transaction();
}

uint8_t mfrc522_read_register(uint8_t reg){
    uint8_t temp;

    spi_begin_transaction();
    PC_ODR &= ~SHIFTL8(CS_PIN); //low
    spi_transfer(reg | 0x80u);
    temp = spi_transfer(0);
    PC_ODR |= SHIFTL8(CS_PIN); //high
    spi_end_transaction();

    return temp;
}

static void mfrc522_set_bit_mask(uint8_t reg, uint8_t mask){
    uint8_t tmp;

    tmp = mfrc522_read_register(reg);
    mfrc522_write_register(reg, tmp | mask);
}

static inline void mfrc522_reset(){
    uint8_t count;
    mfrc522_write_register(MFRC522_REG_COMMAND, PCD_RESETPHASE);
    
    count = 0;
    do{
        delay(50);
    }while((mfrc522_read_register(MFRC522_REG_COMMAND) & SHIFTL8(4)) && (++count)<3);
}

static inline void mfrc522_antenna_on(){
    uint8_t temp;

    temp = mfrc522_read_register(MFRC522_REG_TX_CONTROL);

    if((temp & 0x03u) != 0x03u){
        mfrc522_write_register(MFRC522_REG_TX_CONTROL, temp | 0x03u);
    }
}

inline void mfrc522_init(){
    PC_DDR |= SHIFTL8(CS_PIN); //output
    PC_CR1 |= SHIFTL8(CS_PIN); //pushpull
    PC_ODR |= SHIFTL8(CS_PIN); //high


    mfrc522_reset();
    mfrc522_write_register(MFRC522_REG_TX_MODE, 0);
    mfrc522_write_register(MFRC522_REG_RX_MODE, 0);
    mfrc522_write_register(MFRC522_REG_MOD_WIDTH, 0x26u);

    mfrc522_write_register(MFRC522_REG_T_MODE, 0x80u);
    mfrc522_write_register(MFRC522_REG_T_PRESCALER, 0xA9u);
    mfrc522_write_register(MFRC522_REG_T_RELOAD_H, 0x03u);
    mfrc522_write_register(MFRC522_REG_T_RELOAD_L, 0xE8u);

    mfrc522_write_register(MFRC522_REG_TX_AUTO, 0x40u);
    mfrc522_write_register(MFRC522_REG_MODE, 0x3Du);

    mfrc522_antenna_on();

    //Set maximum gain
    //mfrc522_write_register(MFRC522_REG_RF_CFG, 0x70u);
}
