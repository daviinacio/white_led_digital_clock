#ifndef WLDC_LIGHT_SENSOR_H
#define WLDC_LIGHT_SENSOR_H
#include <Thread.h>
#include "display.hpp"

#define LDR_DEFAULT_PIN A2
#define LDR_DEFAULT_BUFFER_LENGTH 16
#define LDR_DEFAULT_DURATION 2000

class LightSensor : public Thread {
protected:
  int pin;
  long duration = 0;
public:
  LightSensor();
  LightSensor(int _pin);
  LightSensor(int _pin, unsigned long _duration);
  LightSensor(int _pin, unsigned long _duration, int _buffer_length);
  void run();
};

LightSensor::LightSensor(int _pin, unsigned long _duration, int _buffer_length){
  
}

LightSensor::LightSensor(int _pin, unsigned long _duration) {
  LightSensor(_pin, _duration, LDR_DEFAULT_BUFFER_LENGTH);
}

LightSensor::LightSensor(int _pin){
  LightSensor(_pin, LDR_DEFAULT_DURATION);
}

LightSensor::LightSensor(){
  LightSensor(LDR_DEFAULT_PIN);
}

void LightSensor::run(){
  //Display.setBrightness(0);
}



#endif