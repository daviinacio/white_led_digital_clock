#include <Thread.h>

#ifndef WLDC_BUZZER_DRIVER_H
#define WLDC_BUZZER_DRIVER_H

#define BZ_PIN PB1

class Buzzer {
protected:

public:
  void begin(){
    // Pin Mode
    DDRB ^= (1 << BZ_PIN);

    // Stop Timer/Counter1 clock by setting the clock source to none
    TCCR1B &= ~(_BV(CS10) | _BV(CS11) | _BV(CS12));
    
    // Clear the Timer/Counter 1 control registers A and B
    TCCR1A = 0; 
    TCCR1B = 0;
    TCNT1  = 0;

    // Turn on CTC mode (WGM12 bit set)
    TCCR1B |= (1 << WGM12);
  }
  
  void tone(uint16_t frequency_hz){
    if(frequency_hz == 0) return mute();
    // Set COM1A0 to toggle OC1A (pin 9) on Compare Match
    TCCR1A |= (1 << COM1A0); 
    TCCR1B |= (1 << CS11) | (1 << CS10);

    uint16_t calculated_ocr = (F_CPU / (2L * 64L * frequency_hz)) -1L;
    range(calculated_ocr, 1, 65535);

    OCR1A = (uint16_t) calculated_ocr;
    TCNT1  = 0;
  }

  void mute(){
    TCCR1A &= ~(1 << COM1A0);
    PORTB &= ~(1 << BZ_PIN);
  };
};

Buzzer buzzer;

#endif
