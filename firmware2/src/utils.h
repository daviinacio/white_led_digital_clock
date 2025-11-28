#include <Arduino.h>

#ifndef WLDC_UTILS_H
#define WLDC_UTILS_H

void range(unsigned short& value, unsigned short min, unsigned short max);

void increment(unsigned short& value, unsigned short min, unsigned short max, bool loop, unsigned short mult);

void increment(unsigned short& value, unsigned short min, unsigned short max, bool loop);

void decrement(unsigned short& value, unsigned short min, unsigned short max, bool loop, unsigned short mult);

void decrement(unsigned short& value, unsigned short min, unsigned short max, bool loop);

unsigned short month_last_day(unsigned short year, unsigned short month);

void debug_mode_print_key_name(int key);

#endif
