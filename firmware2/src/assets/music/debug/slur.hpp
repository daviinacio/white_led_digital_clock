#include "assets/music/base.h"

static const uint8_t debug__slur__voice_1[] PROGMEM = {
  MUSIC_BPM,    120,
  MUSIC_BEATS,  2,

  NOTE_C1,  WHOLE_NOTE_SLUR,

  NOTE_A3,  EIGHTH_NOTE_SLUR,

  REPEAT_START, 12,

    NOTE_A3, SIXTEENTH_NOTE_SLUR,
    NOTE_A5, SIXTEENTH_NOTE_SLUR,

  REPEAT_END,

  // ...

  MUSIC_END
};

static const uint8_t debug__slur__voice_2[] PROGMEM = {
  NOTE_C4,  WHOLE_NOTE_SLUR,
  NOTE_C1,  WHOLE_NOTE_SLUR,
  NOTE_A4,  WHOLE_NOTE,
  NOTE_A4,  WHOLE_NOTE_SLUR,
  NOTE_G4,  WHOLE_NOTE_SLUR,
  NOTE_C1,  WHOLE_NOTE,

  // ...

  MUSIC_END
};


const uint8_t* const debug__slur[] PROGMEM = {
  debug__slur__voice_1,
  debug__slur__voice_2,
  music_sheet_end
};
