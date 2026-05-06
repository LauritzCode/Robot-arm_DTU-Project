#pragma once 



// Master
void spi_master_init();
unsigned char putcSPI_master(unsigned char cx);
unsigned char getcSPI_master(void);

// Slave

void spi_slave_init();
void putcSPI_slave(unsigned char cx);
unsigned char getcSPI_slave(void);

