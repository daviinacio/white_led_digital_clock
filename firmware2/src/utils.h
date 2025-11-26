#include <Arduino.h>

#ifndef WLDC_UTILS_H
#define WLDC_UTILS_H

void range(unsigned short& value, unsigned short min, unsigned short max){
  if(value > max)
    value = max;
  else if(value < min)
    value = min;
}

void increment(unsigned short& value, unsigned short min, unsigned short max, bool loop, unsigned short mult){
  value = value < max ? value + min(max - value, mult) : (
    loop ? min : max
  );
}

void increment(unsigned short& value, unsigned short min, unsigned short max, bool loop){
  increment(value, min, max, loop, 1);
}

void decrement(unsigned short& value, unsigned short min, unsigned short max, bool loop, unsigned short mult){
  value = value > min ? value - min(value, mult) : (
    loop ? max : min
  );
}

void decrement(unsigned short& value, unsigned short min, unsigned short max, bool loop){
  decrement(value, min, max, loop, 1);
}

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

// void print_debug(){
//   #if WLDC_DISPLAY_DEBUG_MODE
//   Serial.print();
//   #endif
// }

#endif
