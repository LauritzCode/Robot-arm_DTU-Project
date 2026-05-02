#pragma once 
#include <avr/io.h> 

void adc_init();

void select_ch(uint8_t channel);

uint16_t analog_read(uint8_t channel);

