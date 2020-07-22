#ifndef STM8_SPI_H
#define STM8_SPI_H

#include "common.h"

//Registers
#define SPI_CR1 __RMM(0x200)
#define SPI_CR2 __RMM(0x201)
#define SPI_SR  __RMM(0x203)
#define SPI_DR  __RMM(0x204)


//Registers' bits
#define SPI_CR1_MSTR SHIFTL8(2)
#define SPI_CR1_SPE  SHIFTL8(6)
#define SPI_CR2_SSM  SHIFTL8(1) //<<CS managed in SW
#define SPI_CR2_SSI  SHIFTL8(0)
#define SPI_SR_BSY   SHIFTL8(7)
#define SPI_SR_TXE   SHIFTL8(1)
#define SPI_SR_RXNE  SHIFTL8(0)


void    spi_begin_transaction();
uint8_t spi_transfer(uint8_t data);
void    spi_end_transaction();


#endif