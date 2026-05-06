#pragma once
#include <stdint.h>

extern volatile char rxBuf[];
extern volatile uint8_t rxIndex;
extern volatile uint8_t receiveFlag;

void uart_init();
unsigned char USART_Receive(void);
void uart_transmit();
void uart_print(const char *str);
void handle_command(unsigned char cmd);
int uart_receive_string(char *p, int max);