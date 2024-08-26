#ifndef WLDC_UTILS_H
#define WLDC_UTILS_H

void range(unsigned short& value, unsigned short min, unsigned short max){
  if(value > max)
    value = max;
  else if(value < min)
    value = min;
}

void increment(unsigned short& value, unsigned short min, unsigned short max, bool loop){
  if(loop)
    value = value < max ? value + 1 : min;
  else
    value = value < max ? value + 1 : value;
}

void decrement(unsigned short& value, unsigned short min, unsigned short max, bool loop){
  if(loop)
    value = value > min ? value - 1 : max;
  else
    value = value > min ? value - 1 : value;
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
}

#endif