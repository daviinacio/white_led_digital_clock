#ifndef WLDC_MUSIC_BASE_H
#define WLDC_MUSIC_BASE_H

static const uint8_t music_sheet_end[] PROGMEM {};

#endif


// Metadata
#define MUSIC_END 0
#define MUSIC_BPM 20
#define MUSIC_BEATS 21

#define REPEAT_START 30
#define REPEAT_END 31
#define REPEAT_ENDING 32

// Note modifiers
#define TIE     40
#define SLUR    41

// Timing
#define WHOLE_NOTE                    50
#define HALF_NOTE                     51
#define QUARTER_NOTE                  52
#define EIGHTH_NOTE                   53
#define SIXTEENTH_NOTE                54
#define THIRTY_SECOND_NOTE            55
#define SIXTY_FOURTH_NOTE             56

#define WHOLE_NOTE_DOTTED             57
#define HALF_NOTE_DOTTED              58
#define QUARTER_NOTE_DOTTED           59
#define EIGHTH_NOTE_DOTTED            60
#define SIXTEENTH_NOTE_DOTTED         61
#define THIRTY_SECOND_NOTE_DOTTED     62
#define SIXTY_FOURTH_NOTE_DOTTED      63

#define WHOLE_NOTE_TIE                WHOLE_NOTE, TIE
#define HALF_NOTE_TIE                 HALF_NOTE, TIE
#define QUARTER_NOTE_TIE              QUARTER_NOTE, TIE
#define EIGHTH_NOTE_TIE               EIGHTH_NOTE, TIE
#define SIXTEENTH_NOTE_TIE            SIXTEENTH_NOTE, TIE
#define THIRTY_SECOND_NOTE_TIE        THIRTY_SECOND_NOTE, TIE
#define SIXTY_FOURTH_NOTE_TIE         SIXTY_FOURTH_NOTE, TIE

#define WHOLE_NOTE_DOTTED_TIE         WHOLE_NOTE_DOTTED, TIE
#define HALF_NOTE_DOTTED_TIE          HALF_NOTE_DOTTED, TIE
#define QUARTER_NOTE_DOTTED_TIE       QUARTER_NOTE_DOTTED, TIE
#define EIGHTH_NOTE_DOTTED_TIE        EIGHTH_NOTE_DOTTED, TIE
#define SIXTEENTH_NOTE_DOTTED_TIE     SIXTEENTH_NOTE_DOTTED, TIE
#define THIRTY_SECOND_NOTE_DOTTED_TIE THIRTY_SECOND_NOTE_DOTTED, TIE
#define SIXTY_FOURTH_NOTE_DOTTED_TIE  SIXTY_FOURTH_NOTE_DOTTED, TIE

// Rest
#define REST_NOTE                     70
#define WHOLE_REST                    REST_NOTE, WHOLE_NOTE
#define HALF_REST                     REST_NOTE, HALF_NOTE
#define QUARTER_REST                  REST_NOTE, QUARTER_NOTE
#define EIGHTH_REST                   REST_NOTE, EIGHTH_NOTE
#define SIXTEENTH_REST                REST_NOTE, SIXTEENTH_NOTE
#define THIRTY_SECOND_REST            REST_NOTE, THIRTY_SECOND_NOTE
#define SIXTY_FOURTH_REST             REST_NOTE, SIXTY_FOURTH_NOTE

#define WHOLE_REST_DOTTED             REST_NOTE, WHOLE_NOTE_DOTTED
#define HALF_REST_DOTTED              REST_NOTE, HALF_NOTE_DOTTED
#define QUARTER_REST_DOTTED           REST_NOTE, QUARTER_NOTE_DOTTED
#define EIGHTH_REST_DOTTED            REST_NOTE, EIGHTH_NOTE_DOTTED
#define SIXTEENTH_REST_DOTTED         REST_NOTE, SIXTEENTH_NOTE_DOTTED
#define THIRTY_SECOND_REST_DOTTED     REST_NOTE, THIRTY_SECOND_NOTE_DOTTED
#define SIXTY_FOURTH_REST_DOTTED      REST_NOTE, SIXTY_FOURTH_NOTE_DOTTED

// Notes
#define NOTE_A   101
#define NOTE_As  102
#define NOTE_Bb  102
#define NOTE_B   103
#define NOTE_C   104
#define NOTE_Cs  105
#define NOTE_Db  105
#define NOTE_D   106
#define NOTE_Ds  107
#define NOTE_Eb  107
#define NOTE_E   108
#define NOTE_F   109
#define NOTE_Fs  110
#define NOTE_Gb  110
#define NOTE_G   111
#define NOTE_Gs  112
#define NOTE_Ab  112

#define NOTE_C1   NOTE_C,  1
#define NOTE_Cs1  NOTE_Cs, 1
#define NOTE_Db1  NOTE_Db, 1
#define NOTE_D1   NOTE_D,  1
#define NOTE_Ds1  NOTE_Ds, 1
#define NOTE_Eb1  NOTE_Eb, 1
#define NOTE_E1   NOTE_E,  1
#define NOTE_F1   NOTE_F,  1
#define NOTE_Fs1  NOTE_Fs, 1
#define NOTE_Gb1  NOTE_Gb, 1
#define NOTE_G1   NOTE_G,  1
#define NOTE_Gs1  NOTE_Gs, 1
#define NOTE_Ab1  NOTE_Ab, 1
#define NOTE_A1   NOTE_A,  1
#define NOTE_As1  NOTE_As, 1
#define NOTE_Bb1  NOTE_Bb, 1
#define NOTE_B1   NOTE_B,  1

#define NOTE_C2   NOTE_C,  2
#define NOTE_Cs2  NOTE_Cs, 2
#define NOTE_Db2  NOTE_Db, 2
#define NOTE_D2   NOTE_D,  2
#define NOTE_Ds2  NOTE_Ds, 2
#define NOTE_Eb2  NOTE_Eb, 2
#define NOTE_E2   NOTE_E,  2
#define NOTE_F2   NOTE_F,  2
#define NOTE_Fs2  NOTE_Fs, 2
#define NOTE_Gb2  NOTE_Gb, 2
#define NOTE_G2   NOTE_G,  2
#define NOTE_Gs2  NOTE_Gs, 2
#define NOTE_Ab2  NOTE_Ab, 2
#define NOTE_A2   NOTE_A,  2
#define NOTE_As2  NOTE_As, 2
#define NOTE_Bb2  NOTE_Bb, 2
#define NOTE_B2   NOTE_B,  2

#define NOTE_C3   NOTE_C,  3
#define NOTE_Cs3  NOTE_Cs, 3
#define NOTE_Db3  NOTE_Db, 3
#define NOTE_D3   NOTE_D,  3
#define NOTE_Ds3  NOTE_Ds, 3
#define NOTE_Eb3  NOTE_Eb, 3
#define NOTE_E3   NOTE_E,  3
#define NOTE_F3   NOTE_F,  3
#define NOTE_Fs3  NOTE_Fs, 3
#define NOTE_Gb3  NOTE_Gb, 3
#define NOTE_G3   NOTE_G,  3
#define NOTE_Gs3  NOTE_Gs, 3
#define NOTE_Ab3  NOTE_Ab, 3
#define NOTE_A3   NOTE_A,  3
#define NOTE_As3  NOTE_As, 3
#define NOTE_Bb3  NOTE_Bb, 3
#define NOTE_B3   NOTE_B,  3

#define NOTE_C4   NOTE_C,  4
#define NOTE_Cs4  NOTE_Cs, 4
#define NOTE_Db4  NOTE_Db, 4
#define NOTE_D4   NOTE_D,  4
#define NOTE_Ds4  NOTE_Ds, 4
#define NOTE_Eb4  NOTE_Eb, 4
#define NOTE_E4   NOTE_E,  4
#define NOTE_F4   NOTE_F,  4
#define NOTE_Fs4  NOTE_Fs, 4
#define NOTE_Gb4  NOTE_Gb, 4
#define NOTE_G4   NOTE_G,  4
#define NOTE_Gs4  NOTE_Gs, 4
#define NOTE_Ab4  NOTE_Ab, 4
#define NOTE_A4   NOTE_A,  4
#define NOTE_As4  NOTE_As, 4
#define NOTE_Bb4  NOTE_Bb, 4
#define NOTE_B4   NOTE_B,  4

#define NOTE_C5   NOTE_C,  5
#define NOTE_Cs5  NOTE_Cs, 5
#define NOTE_Db5  NOTE_Db, 5
#define NOTE_D5   NOTE_D,  5
#define NOTE_Ds5  NOTE_Ds, 5
#define NOTE_Eb5  NOTE_Eb, 5
#define NOTE_E5   NOTE_E,  5
#define NOTE_F5   NOTE_F,  5
#define NOTE_Fs5  NOTE_Fs, 5
#define NOTE_Gb5  NOTE_Gb, 5
#define NOTE_G5   NOTE_G,  5
#define NOTE_Gs5  NOTE_Gs, 5
#define NOTE_Ab5  NOTE_Ab, 5
#define NOTE_A5   NOTE_A,  5
#define NOTE_As5  NOTE_As, 5
#define NOTE_Bb5  NOTE_Bb, 5
#define NOTE_B5   NOTE_B,  5

#define NOTE_C6   NOTE_C,  6
#define NOTE_Cs6  NOTE_Cs, 6
#define NOTE_Db6  NOTE_Db, 6
#define NOTE_D6   NOTE_D,  6
#define NOTE_Ds6  NOTE_Ds, 6
#define NOTE_Eb6  NOTE_Eb, 6
#define NOTE_E6   NOTE_E,  6
#define NOTE_F6   NOTE_F,  6
#define NOTE_Fs6  NOTE_Fs, 6
#define NOTE_Gb6  NOTE_Gb, 6
#define NOTE_G6   NOTE_G,  6
#define NOTE_Gs6  NOTE_Gs, 6
#define NOTE_Ab6  NOTE_Ab, 6
#define NOTE_A6   NOTE_A,  6
#define NOTE_As6  NOTE_As, 6
#define NOTE_Bb6  NOTE_Bb, 6
#define NOTE_B6   NOTE_B,  6

#define NOTE_C7   NOTE_C,  7
#define NOTE_Cs7  NOTE_Cs, 7
#define NOTE_Db7  NOTE_Db, 7
#define NOTE_D7   NOTE_D,  7
#define NOTE_Ds7  NOTE_Ds, 7
#define NOTE_Eb7  NOTE_Eb, 7
#define NOTE_E7   NOTE_E,  7
#define NOTE_F7   NOTE_F,  7
#define NOTE_Fs7  NOTE_Fs, 7
#define NOTE_Gb7  NOTE_Gb, 7
#define NOTE_G7   NOTE_G,  7
#define NOTE_Gs7  NOTE_Gs, 7
#define NOTE_Ab7  NOTE_Ab, 7
#define NOTE_A7   NOTE_A,  7
#define NOTE_As7  NOTE_As, 7
#define NOTE_Bb7  NOTE_Bb, 7
#define NOTE_B7   NOTE_B,  7

