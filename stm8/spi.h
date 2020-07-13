#ifndef STM8_SPI_H
#define STM8_SPI_H

#include "common.h"

//Registers
#define SPI_CR1 __RMM(0x200)
#define SPI_CR2 __RMM(0x201)
#define SPI_SR  __RMM(0x203)
#define SPI_DR  __RMM(0x204)


//Registers' bits
#define SPI_CR1_MSTR 0x04
#define SPI_CR1_SPE  0x40
#define SPI_CR2_SSM  0x02 //<<CS managed in SW
#define SPI_CR2_SSI  0x01
#define SPI_CR2_BDM  0x80 //<<bidirectional on single line
#define SPI_CR2_BDOE 0x40 //<<direction select
#define SPI_SR_TXE   0x02
#define SPI_SR_RXNE  0x01


void    spi_init();
void    spi_write(uint8_t);
uint8_t spi_read();


#endif