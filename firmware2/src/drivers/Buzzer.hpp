#ifndef WLDC_BUZZER_DRIVER_H
#define WLDC_BUZZER_DRIVER_H

#define BZ_PIN PB1
#define BZ_MAX_VOICES  2

class Buzzer {
protected:

  volatile uint16_t phase[BZ_MAX_VOICES];
  volatile uint16_t step[BZ_MAX_VOICES];

  void setup_timer1(){
    cli();

    // Fast PWM 8-bit, OC1A enabled
    TCCR1A = (1 << WGM10) | (1 << COM1A1);
    TCCR1B = (1 << WGM12) | (1 << CS10);

    OCR1A = 127;

    // 31.25 kHz interrupt
    TIMSK1 = (1 << OCIE1A);

    sei();
  }

  // bool has_active_voice(){
  //   for (uint8_t i = 0; i < BZ_MAX_VOICES; i++)
  //     if (step[i] != 0)
  //       return true;
  //   return false;
  // }

  // void update_timer1_state(){
  //   if (has_active_voice()){
  //     // enable interrupt
  //     TIMSK1 |= (1 << OCIE1A);
  //   }
  //   else {
  //     // disable interrupt
  //     TIMSK1 &= ~(1 << OCIE1A);

  //     // force silence (mid-point)
  //     OCR1A = 127;
  //   }
  // }

  uint16_t calc_freq_to_step(float frequency){
    return (uint32_t)(frequency * 65536.0 / 31250.0);
  }

public:
  void begin(){
    // Pin Mode
    DDRB |= (1 << BZ_PIN);

    for (uint8_t i = 0; i < BZ_MAX_VOICES; i++) {
      phase[i] = 0;
      step[i]  = 0;
    }

    setup_timer1();
  }
  
  void tone(uint16_t frequency_hz, uint8_t voice = 0){
    // Serial.print("tone ? ");
    // Serial.print("v=");
    // Serial.print(voice);
    // Serial.print(" f=");
    // Serial.print(frequency_hz);
    // Serial.println(";");

    step[voice] = calc_freq_to_step(frequency_hz);
    // update_timer1_state();
  }

  void mute(){
    for (uint8_t i = 0; i < BZ_MAX_VOICES; i++)
      mute(i);
    // update_timer1_state();
  }

  void mute(uint8_t voice){
    step[voice] = 0;

    // Serial.print("mute ? ");
    // Serial.print("v=");
    // Serial.print(voice);
    // Serial.println(";");
  }

  void run(){
    uint16_t sum = 0;

    // ---- Generate + Mix all voices ----
    for (uint8_t i = 0; i < BZ_MAX_VOICES; i++)
    {
        phase[i] += step[i];

        // Square wave from MSB
        sum += (phase[i] & 0x8000) ? 255 : 0;
    }

    // ---- Scale to 0–255 ----
    // sum max = 255 * BZ_MAX_VOICES
    // multiply by 256/BZ_MAX_VOICES → divide by BZ_MAX_VOICES
    // uint16_t mix = (sum * (256 / BZ_MAX_VOICES)) >> 8;
    uint16_t mix = ((sum / BZ_MAX_VOICES) * 128) >> 8;

    OCR1A = mix;
  };
};

Buzzer buzzer;

ISR(TIMER1_COMPA_vect) {
  buzzer.run();
}

#endif



// #include <Thread.h>

// #ifndef WLDC_BUZZER_DRIVER_H
// #define WLDC_BUZZER_DRIVER_H

// #define BZ_PIN PB1

// class Buzzer {
// protected:

// public:
//   void begin(){
//     // Pin Mode
//     DDRB ^= (1 << BZ_PIN);

//     // Stop Timer/Counter1 clock by setting the clock source to none
//     TCCR1B &= ~(_BV(CS10) | _BV(CS11) | _BV(CS12));
    
//     // Clear the Timer/Counter 1 control registers A and B
//     TCCR1A = 0; 
//     TCCR1B = 0;
//     TCNT1  = 0;

//     // Turn on CTC mode (WGM12 bit set)
//     TCCR1B |= (1 << WGM12);
//   }
  
//   void tone(uint16_t frequency_hz, uint8_t voice = 0){
//     if(frequency_hz == 0) return mute();
//     // Set COM1A0 to toggle OC1A (pin 9) on Compare Match
//     TCCR1A |= (1 << COM1A0); 
//     TCCR1B |= (1 << CS11) | (1 << CS10);

//     uint16_t calculated_ocr = (F_CPU / (2L * 64L * frequency_hz)) -1L;
//     range(calculated_ocr, 1, 65535);

//     OCR1A = (uint16_t) calculated_ocr;
//     TCNT1  = 0;
//   }

//   void mute(){
//     TCCR1A &= ~(1 << COM1A0);
//     PORTB &= ~(1 << BZ_PIN);
//   };
// };

// Buzzer buzzer;

// #endif
