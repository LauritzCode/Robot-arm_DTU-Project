#include "joystick.h"
#include "avr/interrupt.h"
#include "setup/macros.h"
#include <avr/io.h>
#include "analog_read.h"




void joystick_init(Joystick *j, uint8_t ch_x, uint8_t ch_y, uint8_t pin_btn, int16_t deadzone) {

    j->ch_x = ch_x;
    j->ch_y = ch_y;
    j->deadzone = deadzone;
    j->btn = false;
    j->pin_btn = pin_btn; 
    // Forbin j1 (A0 & A1) til D2 og j2 (A2 & A3) til D3

    if(pin_btn == 2) {
        CLRBIT(DDRE, PE4);
        SETBIT(PORTE, PE4);
    }

    if(pin_btn == 3) {
    CLRBIT(DDRE, PE5);
    SETBIT(PORTE, PE5);
    }
}

void joystick_update(Joystick *j) {

    int16_t a_x = analog_read(j->ch_x); 
    int16_t a_y = analog_read(j->ch_y);  

        if(a_x < 512+j->deadzone && a_x > 512-j->deadzone) a_x = 512;
        if(a_y < 512+j->deadzone && a_y > 512-j->deadzone) a_y = 512;
        
        j->x = a_x - 512;
        j->y = a_y - 512;

    switch(j->pin_btn) {
        case 2: j->btn = !(PINE & (1 << PE4)); break;
        case 3: j->btn = !(PINE & (1 << PE5)); break;
    }
    
    
}


