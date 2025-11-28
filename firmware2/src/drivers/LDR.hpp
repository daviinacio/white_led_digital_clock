#include <Arduino.h>
#include <Thread.h>
#include <avr/io.h>

#include "lib/Buffer.h"

#ifndef WLDC_LDR_DRIVER_H
#define WLDC_LDR_DRIVER_H

#define LDR_MIN 0
#define LDR_MAX 1023

class LdrDriver : public Thread {
protected:
  Buffer buffer;
  unsigned short pin;
  short min, max;

public:
  LdrDriver(
    unsigned short _pin,
    short _min,
    short _max,
    short _initial_value,
    unsigned short _length,
    unsigned short _total_interval
  ): buffer(_length, _initial_value) {
    pin = _pin;
    min = _min;
    max = _max;
    interval = _total_interval / _length;
  };

  void run() override {
    // Make the ldr sensor read
    short read = map(analogRead(pin), LDR_MIN, LDR_MAX, min, max);

    // Avoid out of range problems
    if(read > max) read = max; else
    if(read < min) read = min;

    buffer.insert(read);
    Thread::run();
  };

  short value(){
    return buffer.getAverage();
  }

  void set(short value){
    buffer.fill(value);
  }
};

#endif
