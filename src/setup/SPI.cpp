#include "SPI.h"
#include "macros.h"
#include <Arduino.h>

// Master
void spi_master_init() {

    DDRB |= (1<<PB0) | (1<<PB1) | (1<<PB2);
    SPCR |= (1<<MSTR) | (1<<CPOL) | (1<<SPE);
    SETBIT(SPCR, SPR1); // 250Khz SCLK
    SETBIT(PORTB, PB0);
    
    
}
unsigned char putcSPI_master(unsigned char cx) {

    CLRBIT(PORTB, PB0);
    SPDR = cx;

    while(!(SPSR &(1<<SPIF))); 
    SETBIT(PORTB, PB0);

    return SPDR;
}
unsigned char getcSPI_master(void) {
    CLRBIT(PORTB, PB0);
    SPDR = 0;

    while(!(SPSR &(1<<SPIF)));
    SETBIT(PORTB, PB0);

    return SPDR;
}

// Slave

void spi_slave_init() {

    SETBIT(DDRB, PB3);
    SPCR |= (1 << SPE) | (1 << CPOL);

}

void putcSPI_slave(unsigned char cx) {
    
    char tmp;
    SPDR = cx;

    while(!(SPSR &(1<<SPIF)));

    tmp = SPDR;

}
unsigned char getcSPI_slave(void) {
    while(!(SPSR & (1 << SPIF)));

    return SPDR;
}
