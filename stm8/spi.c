#include "spi.h"

/*
 * Remember that we don't initialize any CS pin here,
 * you are a big man, do it yourself
 */
inline void spi_init(){
    /*
     * Warning, we set the SPI clock @ F_CPU / 2, since
     * the maximum F_CPU should be 16 MHz => 8 MHz.
     * Most SPI devices support a maximum clock speed of
     * 10 MHz, that should always be fine
     */

    SPI_CR1 = SPI_CR1_MSTR | SPI_CR1_SPE;
    SPI_CR2 = SPI_CR2_SSM | SPI_CR2_SSI | SPI_CR2_BDM | SPI_CR2_BDOE;
}

void spi_write(uint8_t data){
    SPI_DR = data;
    while(!(SPI_SR & SPI_SR_TXE));
}

uint8_t spi_read(){
    spi_write(0xFF);
    while(!(SPI_SR & SPI_SR_RXNE));
    return SPI_DR;
}