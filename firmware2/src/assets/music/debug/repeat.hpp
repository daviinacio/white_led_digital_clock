#include "assets/music/base.h"

static const uint8_t debug__repeat__voice_1[] PROGMEM = {
  MUSIC_BPM,    80,
  MUSIC_BEATS,  4,

  REPEAT_START, 4,

    NOTE_A4,  EIGHTH_NOTE,
    NOTE_B4,  EIGHTH_NOTE,

    REPEAT_ENDING, 1, 3,
    
    NOTE_C5,  EIGHTH_NOTE,

    REPEAT_ENDING, 2,
    
    NOTE_D5,  EIGHTH_NOTE,

    REPEAT_ENDING, 4,
    
    NOTE_E5,  EIGHTH_NOTE,

  REPEAT_END,

  // ...

  MUSIC_END
};


const uint8_t* const debug__repeat[] PROGMEM = {
  debug__repeat__voice_1,
  music_sheet_end
};
