#include "assets/music/base.h"

const uint8_t microsoft__windows_xp_shutdown__right_hand[] PROGMEM = {
  MUSIC_BPM,    120,
  MUSIC_BEATS,  4,

  NOTE_Gs5,  EIGHTH_NOTE,
  NOTE_Ds5,  EIGHTH_NOTE,
  NOTE_Gs4,  EIGHTH_NOTE,
  NOTE_As4,  EIGHTH_NOTE_TIE,
  NOTE_As4,  EIGHTH_NOTE,

  MUSIC_END
};

const uint8_t microsoft__windows_xp_shutdown__left_hand[] PROGMEM = {
  NOTE_Gs3,  QUARTER_NOTE_DOTTED,
  NOTE_Ds3,  EIGHTH_NOTE_TIE,
  NOTE_Ds3,  EIGHTH_NOTE,

  MUSIC_END
};


const uint8_t* const microsoft__windows_xp_shutdown[] PROGMEM = {
  microsoft__windows_xp_shutdown__right_hand,
  microsoft__windows_xp_shutdown__left_hand,
  music_sheet_end
};
