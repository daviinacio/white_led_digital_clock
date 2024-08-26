#ifndef WLDC_INFRARED_H
#define WLDC_INFRARED_H

#include <IRremote.hpp>
#include "input.hpp"

#define INFRARED_DEFAULT_INTERVAL 500

#define HEX_KEY_HOME        (value == 0xE51A0707) // Return
#define HEX_KEY_VALUE_UP    (value == 0x9F600707 || value == 0xF8070707) // Arrow Up or  Vol+
#define HEX_KEY_VALUE_DOWN  (value == 0x9E610707 || value == 0xF40B0707) // Arrow Down or Vol-
#define HEX_KEY_FUNC_LEFT   (value == 0x9A650707) // Arrow Left
#define HEX_KEY_FUNC_RIGHT  (value == 0x9D620707) // Arrow Right

class InfraredSensor : public Input {
protected:
  unsigned int pin;
  InputKey readPanel();

public:
  InfraredSensor(unsigned int _pin);
  InfraredSensor(unsigned int _pin, unsigned long _interval);
  void begin();
};


InfraredSensor::InfraredSensor(unsigned int _pin, unsigned long _interval) : Input(_interval) {
  pin = _pin;
}

InfraredSensor::InfraredSensor(unsigned int _pin) : Input(INFRARED_DEFAULT_INTERVAL) {
  pin = _pin;
}

void InfraredSensor::begin(){
  IrReceiver.begin(IR_PIN);
}

InputKey InfraredSensor::readPanel() {
  InputKey key = KEY_DEFAULT;
  if(!IrReceiver.decode()) return key;

  // Read the ir HEX value
  unsigned long value = IrReceiver.decodedIRData.decodedRawData;
  // Receive the next value
  IrReceiver.resume();

  if(HEX_KEY_HOME) key = KEY_HOME;
  else if(HEX_KEY_VALUE_UP) key = KEY_VALUE_UP;
  else if(HEX_KEY_VALUE_DOWN) key = KEY_VALUE_DOWN;
  else if(HEX_KEY_FUNC_LEFT) key = KEY_FUNC_LEFT;
  else if(HEX_KEY_FUNC_RIGHT) key = KEY_FUNC_RIGHT;
  
  return key;
}


#endif