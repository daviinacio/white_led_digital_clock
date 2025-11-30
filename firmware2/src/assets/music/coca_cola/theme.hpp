#include "assets/music/base.h"

const uint16_t coca_cola__theme__config[] PROGMEM = {
  112,  // BMP
  2,    // Beats
  2     // Voice count
};

const uint16_t coca_cola__theme__voice_1[] PROGMEM = {
  NOTE_E5,  QUARTER_NOTE,
  NOTE_C5,  QUARTER_NOTE,
  NOTE_D5,  QUARTER_NOTE,
  NOTE_E5,  EIGHTH_NOTE,
  NOTE_C5,  QUARTER_NOTE,

  0,0
};

const uint16_t coca_cola__theme__voice_2[] PROGMEM = {
  NOTE_E4,  QUARTER_NOTE,
  NOTE_C4,  QUARTER_NOTE,
  NOTE_D4,  QUARTER_NOTE,
  NOTE_E4,  EIGHTH_NOTE,
  NOTE_C4,  QUARTER_NOTE,

  0,0
};


const uint16_t* const coca_cola__theme[] PROGMEM = {
  coca_cola__theme__config,
  coca_cola__theme__voice_1,
  coca_cola__theme__voice_2
};
