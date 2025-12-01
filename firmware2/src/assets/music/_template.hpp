#include "assets/music/base.h"

const uint16_t template__chords__config[] PROGMEM = {
  112,  // BMP
  2,    // Beats
  2     // Voice count
};

const uint16_t template__chords__voice_1[] PROGMEM = {
  0,0
};

const uint16_t template__chords__voice_2[] PROGMEM = {
  0,0
};


const uint16_t* const template__chords[] PROGMEM = {
  template__chords__config,
  template__chords__voice_1,
  template__chords__voice_2
};
