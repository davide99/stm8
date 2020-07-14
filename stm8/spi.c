#include "spi.h"

/*
 * Remember that we don't initialize any CS pin here,
 * you are a big man, do it yourself
 */
void spi_begin_transaction(){
    /*
     * Warning, we set the SPI clock @ F_CPU / 2, since
     * the maximum F_CPU should be 16 MHz => 8 MHz.
     * Most SPI devices support a maximum clock speed of
     * 10 MHz, that should always be fine
     */

    SPI_CR2 = SPI_CR2_SSM | SPI_CR2_SSI;
    SPI_CR1 = SPI_CR1_MSTR | SPI_CR1_SPE;
}

uint8_t spi_transfer(uint8_t data){
    SPI_DR = data; //tx
    while(!(SPI_SR & SPI_SR_TXE));

    while(!(SPI_SR & SPI_SR_RXNE));
    return SPI_DR;   //rx
}

void spi_end_transaction(){
    while(!(SPI_SR & SPI_SR_TXE));
    while((SPI_SR & SPI_SR_BSY));
    SPI_CR1 &= ~SPI_CR1_SPE;
}