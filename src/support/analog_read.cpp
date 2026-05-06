#include "analog_read.h"
#include "setup/macros.h"
#include "support/timer_helper.h"
#include <avr/interrupt.h>

volatile uint8_t adc_ready = 0;

void adc_init() {

    SETBIT(ADCSRA, ADPS2); SETBIT(ADCSRA, ADPS1); SETBIT(ADCSRA, ADPS0); // prescaler to 128 -- 125 kHz
    SETBIT(ADCSRA, ADEN); // Enable ADC
    SETBIT(ADMUX, REFS0); // Enable AVCC with external capacitor
    FILLBIT(DIDR0); // Disable digital I/O

    // Interrupt set up

    SETBIT(ADCSRA, ADIE); // Interrupt enable
    initialize_timer(Timer0, 125000);

    
    // ADTS bits for Timer0 Compare Match A
    SETBIT(ADCSRA, ADSC);
    
}


void select_ch(uint8_t channel) {

    ZROBIT(ADMUX); SETBIT(ADMUX, REFS0); ADMUX |= channel; 

}


uint16_t analog_read(uint8_t channel) {

    select_ch(channel);
    // Dummy conversion: AVR ADC mux needs one cycle to settle after channel switch
    SETBIT(ADCSRA, ADSC);
    while(!adc_ready);
    adc_ready = 0;
    SETBIT(ADCSRA, ADSC);
    while(!adc_ready);
    adc_ready = 0;
    return ADC;
}


ISR(ADC_vect) {

    adc_ready = 1;
}

ISR(TIMER0_COMPA_vect) {

}

