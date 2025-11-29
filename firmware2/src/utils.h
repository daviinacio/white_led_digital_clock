#include <Arduino.h>

#ifndef WLDC_UTILS_H
#define WLDC_UTILS_H

template <typename A, typename B, typename C>
void range(A& value, B minv, C maxv);

template <typename A, typename B, typename C>
void increment(A& value, B minv, C maxv, bool loop);

template <typename A, typename B, typename C>
void decrement(A& value, B minv, C maxv, bool loop);

unsigned short month_last_day(unsigned short year, unsigned short month);

void debug_mode_print_key_name(int key);

#endif
