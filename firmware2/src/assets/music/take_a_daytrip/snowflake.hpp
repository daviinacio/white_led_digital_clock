#include "assets/music/base.h"

static const uint8_t take_a_daytrip__snowflake__voice_1[] PROGMEM = {
  MUSIC_BPM,    120,
  MUSIC_BEATS,  4,

  // Rise

  // Xmas

  // Linear

  // Arpeggio

  MUSIC_END
};

static const uint8_t take_a_daytrip__snowflake__voice_2[] PROGMEM = {
  
  // Rest

  // Rest

  // Linear

  // Linear

  MUSIC_END
};


const uint8_t* const take_a_daytrip__snowflake[] PROGMEM = {
  take_a_daytrip__snowflake__voice_1,
  take_a_daytrip__snowflake__voice_2,
  music_sheet_end
};
