#include "utils.h"

template <typename A, typename B, typename C>
void range(A& value, B minv, C maxv){
  A larger_min = static_cast<A>(minv);
  A larger_max = static_cast<A>(maxv);

  value = max(min(value, larger_max), larger_min);
}

template <typename A, typename B, typename C>
void increment(A& value, B minv, C maxv, bool loop){
    // Cast min and max to the same type as value (T) for the comparison and arithmetic
    A larger_min = static_cast<A>(minv);
    A larger_max = static_cast<A>(maxv);

    // Perform the increment logic
    value = (value < larger_max) ? value + 1 : (loop ? larger_min : larger_max);
}

template <typename A, typename B, typename C>
void decrement (A& value, B minv, C maxv, bool loop){
    // Cast min and max to the same type as value (T) for the comparison and arithmetic
    A larger_min = static_cast<A>(minv);
    A larger_max = static_cast<A>(maxv);

  value = value > larger_min ? value - 1 : (loop ? larger_max : larger_min);
}

// Explicit instantiations
template void range<double, int, int>(double&, int, int);
template void range<unsigned short, int, unsigned short>(unsigned short&, int, unsigned short);
template void range<unsigned int, int, int>(unsigned int&, int, int);

template void increment<uint8_t, int, int>(uint8_t&, int, int, bool);
template void increment<unsigned short, int, int>(unsigned short&, int, int, bool);
template void increment<unsigned short, int, unsigned short>(unsigned short&, int, unsigned short, bool);

template void decrement<uint8_t, int, int>(uint8_t&, int, int, bool);
template void decrement<unsigned short, int, int>(unsigned short&, int, int, bool);
template void decrement<unsigned short, int, unsigned short>(unsigned short&, int, unsigned short, bool);



unsigned short month_last_day(unsigned short year, unsigned short month){
  bool elapse_year = year % 4 == 0;
  range(month, 1, 12);

  switch(month){
    case 1: return 31;
    case 2: return elapse_year ? 29 : 28;
    case 3: return 31;
    case 4: return 30;
    case 5: return 31;
    case 6: return 30;
    case 7: return 31;
    case 8: return 31;
    case 9: return 30;
    case 10: return 31;
    case 11: return 30;
    case 12: return 31;
  }

  return 0;
}

void debug_mode_print_key_name(int key){
  #if WLDC_DISPLAY_DEBUG_MODE
  switch(key){
    case 0x01: Serial.println(F("'home'")); break;
    case 0x02: Serial.println(F("'value up'")); break;
    case 0x03: Serial.println(F("'value down'")); break;
    case 0x04: Serial.println(F("'func left'")); break;
    case 0x05: Serial.println(F("'func right'")); break;
  }
  #endif
}
