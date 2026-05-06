#pragma once
#include <Arduino.h>
#define DEBOUNCE_MS 50

void debounce(uint8_t *status, uint8_t pin);
void print_centered(const char *text, uint8_t row);
