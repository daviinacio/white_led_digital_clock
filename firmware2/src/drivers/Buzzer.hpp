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
    step[voice] = calc_freq_to_step(frequency_hz);
  }

  void mute(){
    for (uint8_t i = 0; i < BZ_MAX_VOICES; i++)
      mute(i);
  }

  void mute(uint8_t voice){
    step[voice] = 0;
    phase[voice] = 0;
  }

  void run(){
    uint16_t sum = 0;

    // ---- Generate + Mix all voices ----
    for (uint8_t i = 0; i < BZ_MAX_VOICES; i++){
        phase[i] += step[i];

        // Square wave from MSB
        sum += (phase[i] & 0x8000) ? 255 : 0;
    }

    // ---- Scale to 0–255 ----
    uint16_t mix = ((sum / BZ_MAX_VOICES) * 128) >> 8;

    OCR1A = mix;
  };
};

Buzzer buzzer;

ISR(TIMER1_COMPA_vect) {
  buzzer.run();
}

#endif
