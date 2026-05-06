#include "helpers.h"
#include "setup/I2C.h" 
#include "setup/ssd1306.h" 
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "macros.h"


void debounce(uint8_t *status, uint8_t pin) {

    static uint8_t last_read = 0;
    static uint8_t state = 0;
    static uint32_t last_change_time = 0;
    uint8_t current_status = ((1 << pin) & PINE) >> pin;
    uint32_t now = millis();

    if(last_read != current_status) {
        last_change_time = now;
        last_read = current_status;
    }

    if(now - last_change_time > 50) {
        state = current_status;
    }

    *status = state;
    }  
    
// Centers text on OLED

void print_centered(const char *text, uint8_t row) {
    int str_len = strlen(text);
    uint8_t col_config = (SCREEN_WIDTH - (CHAR_WIDTH * str_len)) / 16;
    uint8_t col = col_config;
    sendStrXY(text, row, col);
}
