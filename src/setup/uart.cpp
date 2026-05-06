#include "uart.h"
#include "macros.h"
#include "helpers.h"
#include <Arduino.h>
#include "setup/I2C.h" 
#include "setup/ssd1306.h" 

#define BUF_SIZE 16
volatile char rxBuf[BUF_SIZE];
volatile uint8_t rxIndex = 0;
volatile uint8_t receiveFlag = 0;

#define BAUD 19200
#define MYUBRRF F_CPU/8/BAUD -1
#define MYUBRRH F_CPU/16/BAUD -1 // half speed


void uart_init() {

    // Enable TX & RX 

    SETBIT(UCSR0B, RXEN0);
    SETBIT(UCSR0B, TXEN0);
    SETBIT(UCSR0B, RXCIE0); // Enable RX complete interrupt

    // Set UART protocol to be asynchronous
    CLRBIT(UCSR0C, UMSEL01);
    CLRBIT(UCSR0C, UMSEL00);
    
    // Disable PARITY bit
    CLRBIT(UCSR0C, UPM01);
    CLRBIT(UCSR0C, UPM00);
    
    // Set data to 8-bit 
    CLRBIT(UCSR0B, UCSZ02);
    SETBIT(UCSR0C, UCSZ01);
    SETBIT(UCSR0C, UCSZ00);

    // reduce the divisor of the baud rate divider from 16 to 8 
    // effectively doubling the transfer rate for asynchronous communication
    SETBIT(UCSR0A, U2X0); 

    UBRR0H = (MYUBRRF) >> 8;
    UBRR0L = (MYUBRRF);

}
unsigned char USART_Receive(void) {
// Wait for data to be received 
while ( !(UCSR0A & (1<<RXC0)) )
;

return UDR0;
}

void uart_transmit(unsigned char data) {

    // Wait for empty transmit buffer 
    while ( !( UCSR0A & (1<<UDRE0)) )
    ;
    // Put data into buffer, sends the data 
    UDR0 = data;
}

void uart_print(const char *str) {
    while (*str) {
        uart_transmit(*str++);
    }
}

int uart_receive_string(char *p, int max) {
    int i = 0;
    char c;
    while (i < max - 1) {
        c = USART_Receive();
        if (c == '\n' || c == '\r') break;
        p[i++] = c;
    }
    p[i] = '\0';
    return i;
}

ISR(USART0_RX_vect) {
    char c = UDR0;
    if (c == '\n' || c == '\r') {
        if (rxIndex > 0) {          // ignore empty \r\n pair (second character)
            rxBuf[rxIndex] = '\0';
            receiveFlag = 1;
            rxIndex = 0;
        }
    } else if (rxIndex < BUF_SIZE - 1) {
        rxBuf[rxIndex++] = c;
    }
    // else buffer overrun & discard character
}


