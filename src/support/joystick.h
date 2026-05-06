#pragma once
#include "analog_read.h"
#include "timer_helper.h"
#include "setup/pwm.h"


typedef struct {
    uint8_t ch_x;        // ADC channel for X
    uint8_t ch_y;        // ADC channel for Y
    uint8_t pin_btn;     // digital pin for button
    int16_t x;           // centered x value
    int16_t y;           // centered y value
    bool btn;            // button state
    int16_t deadzone;    // deadzone threshold
} Joystick;


void joystick_init(Joystick *j, uint8_t ch_x, uint8_t ch_y, uint8_t pin_btn, int16_t deadzone);

void joystick_update(Joystick *j);

