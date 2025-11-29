#include <Thread.h>

#ifndef WLDC_BUZZER_DRIVER_H
#define WLDC_BUZZER_DRIVER_H

#define BZ_PIN PB1

class Buzzer {
protected:

public:
  void enable(){
    // Set COM1A0 to toggle OC1A (pin 9) on Compare Match
    TCCR1A |= (1 << COM1A0); 
    TCCR1B |= (1 << CS11) | (1 << CS10);
  }

  void disable(){
    TCCR1A &= ~(1 << COM1A0);
    PORTB &= ~(1 << BZ_PIN);
  }

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

  void toneSync(unsigned short frequency_hz, unsigned short interval){
    unsigned int tone_init = millis();
    if(frequency_hz == 0) {
      delay(interval);
    }
    else {
      while((millis() - (tone_init + interval)) & 0x80000000){
        PORTB ^= (1 << BZ_PIN);
        delayMicroseconds(1000000L / (frequency_hz * 2));
      }
    }
    disable();
  }
  
  void tone(uint16_t frequency_hz){
    if(frequency_hz == 0) return stop();
    enable();

    uint16_t calculated_ocr = (F_CPU / (2L * 64L * frequency_hz)) -1L;
    calculated_ocr = min(calculated_ocr, 65535);
    calculated_ocr = max(calculated_ocr, 1);

    OCR1A = (uint16_t) calculated_ocr;
  }

  void stop(){
    disable();
  };
};

Buzzer buzzer;

#endif
