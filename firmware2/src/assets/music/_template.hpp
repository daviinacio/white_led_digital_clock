#include "assets/music/base.h"

static const uint8_t template__chords__voice_1[] PROGMEM = {
  MUSIC_BPM,    80,
  MUSIC_BEATS,  4,

  // ...

  MUSIC_END
};

static const uint8_t template__chords__voice_2[] PROGMEM = {
  // ...

  MUSIC_END
};


const uint8_t* const template__chords[] PROGMEM = {
  template__chords__voice_1,
  template__chords__voice_2,
  music_sheet_end
};
