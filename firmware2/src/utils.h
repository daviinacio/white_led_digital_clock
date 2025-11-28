#include <Arduino.h>

#ifndef WLDC_UTILS_H
#define WLDC_UTILS_H

void range(unsigned short& value, unsigned short min, unsigned short max);

template <typename A, typename B, typename C>
void increment(A& value, B min, C max, bool loop);

template <typename A, typename B, typename C>
void decrement(A& value, B min, C max, bool loop);

unsigned short month_last_day(unsigned short year, unsigned short month);

void debug_mode_print_key_name(int key);

#endif
