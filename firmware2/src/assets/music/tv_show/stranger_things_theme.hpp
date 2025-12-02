/**
 * https://musescore.com/rileyapperson/scores/2462606
 */


#include "assets/music/base.h"

static const uint8_t tv_show__stranger_things_theme__right_hand[] PROGMEM = {
  MUSIC_BPM,    80,
  MUSIC_BEATS,  4,

  WHOLE_REST,

  REPEAT_START, 30,

  NOTE_C3,  SIXTEENTH_NOTE,
  NOTE_E3,  SIXTEENTH_NOTE,
  NOTE_G3,  SIXTEENTH_NOTE,
  NOTE_B3,  SIXTEENTH_NOTE,
  NOTE_C4,  SIXTEENTH_NOTE,
  NOTE_B3,  SIXTEENTH_NOTE,
  NOTE_G3,  SIXTEENTH_NOTE,
  NOTE_E3,  SIXTEENTH_NOTE,
  
  REPEAT_END,

  NOTE_E2,  WHOLE_NOTE,

  NOTE_E2,  SIXTEENTH_NOTE,
  NOTE_G2,  SIXTEENTH_NOTE,
  NOTE_B2,  SIXTEENTH_NOTE,
  NOTE_C3,  SIXTEENTH_NOTE,
  NOTE_B2,  SIXTEENTH_NOTE,
  NOTE_G2,  SIXTEENTH_NOTE,
  NOTE_E2,  EIGHTH_NOTE_TIE,
  NOTE_E2,  HALF_NOTE,

  MUSIC_END
};

static const uint8_t tv_show__stranger_things_theme__left_hand[] PROGMEM = {
  NOTE_E2,  WHOLE_NOTE,

  WHOLE_REST,

  WHOLE_REST,

  NOTE_E1,  WHOLE_NOTE,

  // 5
  WHOLE_REST,

  NOTE_E2,  QUARTER_NOTE,
  NOTE_G4,  HALF_NOTE_DOTTED,

  HALF_REST_DOTTED,
  NOTE_D2,  QUARTER_NOTE,

  NOTE_E1,  WHOLE_NOTE,

  HALF_REST_DOTTED,
  NOTE_D2,  QUARTER_NOTE,

  // 10
  NOTE_E1,  WHOLE_NOTE_TIE,

  HALF_REST,
  NOTE_D2,  EIGHTH_NOTE_DOTTED,
  NOTE_B1,  EIGHTH_NOTE_TIE,
  SIXTEENTH_REST,
  NOTE_B1,  EIGHTH_NOTE_TIE,

  NOTE_B1,  WHOLE_NOTE,

  WHOLE_REST,

  NOTE_E3,  WHOLE_NOTE,

  // 15
  NOTE_G3,  QUARTER_NOTE,
  NOTE_E3,  HALF_NOTE_DOTTED_TIE,

  NOTE_E3,  HALF_NOTE,
  NOTE_D4,  QUARTER_NOTE,
  NOTE_B2,  EIGHTH_NOTE,
  NOTE_B2,  EIGHTH_NOTE_TIE,

  NOTE_B2,  WHOLE_NOTE,

  WHOLE_REST,

  MUSIC_END
};


const uint8_t* const tv_show__stranger_things_theme[] PROGMEM = {
  tv_show__stranger_things_theme__right_hand,
  tv_show__stranger_things_theme__left_hand,
  music_sheet_end
};
