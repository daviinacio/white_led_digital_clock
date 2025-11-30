#include "assets/music/base.h"

const uint16_t unknown__la_cucaracha__config[] PROGMEM = {
  131,  // BMP
  2,    // Beats
  1     // Voice count
};

const uint16_t unknown__la_cucaracha__voice_1[] PROGMEM = {
  // First part

  NOTE_C4,  EIGHTH_NOTE,
  NOTE_C4,  EIGHTH_NOTE,
  NOTE_C4,  EIGHTH_NOTE,
  NOTE_F4,  QUARTER_NOTE_DOTTED,
  NOTE_A4,  QUARTER_NOTE,

  NOTE_C4,  EIGHTH_NOTE,
  NOTE_C4,  EIGHTH_NOTE,
  NOTE_C4,  EIGHTH_NOTE,
  NOTE_F4,  QUARTER_NOTE_DOTTED,
  NOTE_A4,  HALF_NOTE_DOTTED,

  NOTE_F4,  QUARTER_NOTE,
  NOTE_F4,  EIGHTH_NOTE,
  NOTE_E4,  EIGHTH_NOTE,
  NOTE_E4,  EIGHTH_NOTE,
  NOTE_D4,  EIGHTH_NOTE,
  NOTE_D4,  EIGHTH_NOTE,
  NOTE_C4,  HALF_NOTE_DOTTED,

  // Second part

  NOTE_C4,  EIGHTH_NOTE,
  NOTE_C4,  EIGHTH_NOTE,
  NOTE_C4,  EIGHTH_NOTE,
  NOTE_E4,  QUARTER_NOTE_DOTTED,
  NOTE_G4,  QUARTER_NOTE,

  NOTE_C4,  EIGHTH_NOTE,
  NOTE_C4,  EIGHTH_NOTE,
  NOTE_C4,  EIGHTH_NOTE,
  NOTE_E4,  QUARTER_NOTE_DOTTED,
  NOTE_G4,  HALF_NOTE_DOTTED,

  NOTE_C5,  QUARTER_NOTE,
  NOTE_D5,  EIGHTH_NOTE,
  NOTE_C5,  EIGHTH_NOTE,
  NOTE_As4, EIGHTH_NOTE,
  NOTE_A4,  EIGHTH_NOTE,
  NOTE_G4,  EIGHTH_NOTE,
  NOTE_F4,  HALF_NOTE_DOTTED,

  0,0
};

const uint16_t* const unknown__la_cucaracha[] PROGMEM = {
  unknown__la_cucaracha__config,
  unknown__la_cucaracha__voice_1
};
