#include <Arduino.h>

#define NOTE_MT 0

#define NOTE_C3   130
#define NOTE_Cs3  138
#define NOTE_D3   146
#define NOTE_Ds3  155
#define NOTE_E3   164
#define NOTE_F3   174
#define NOTE_Fs3  185
#define NOTE_G3   196
#define NOTE_Gs3  208
#define NOTE_A3   220
#define NOTE_As3  233
#define NOTE_B3   246

#define NOTE_C4   261
#define NOTE_Cs4  277
#define NOTE_D4   293
#define NOTE_Ds4  311
#define NOTE_E4   329
#define NOTE_F4   349
#define NOTE_Fs4  369
#define NOTE_G4   392
#define NOTE_Gs4  415
#define NOTE_A4   440
#define NOTE_As4  466
#define NOTE_B4   493

#define NOTE_C5   523
#define NOTE_Cs5  554
#define NOTE_D5   587
#define NOTE_Ds5  622
#define NOTE_E5   659
#define NOTE_F5   698
#define NOTE_Fs5  739
#define NOTE_G5   784
#define NOTE_Gs5  830
#define NOTE_A5   880
#define NOTE_As5  932
#define NOTE_B5   987

#define NOTE_C6   1046
#define NOTE_Cs6  1108
#define NOTE_D6   1174
#define NOTE_Ds6  1244
#define NOTE_E6   1318
#define NOTE_F6   1396
#define NOTE_Fs6  1479
#define NOTE_G6   1567
#define NOTE_Gs6  1661
#define NOTE_A6   1760
#define NOTE_As6  1864
#define NOTE_B6   1975

#define NOTE_C7   2093
#define NOTE_Cs7  2217
#define NOTE_D7   2349
#define NOTE_Ds7  2489
#define NOTE_E7   2637
#define NOTE_F7   2793
#define NOTE_Fs7  2959
#define NOTE_G7   3135
#define NOTE_Gs7  3322
#define NOTE_A7   3520
#define NOTE_As7  3729
#define NOTE_B7   3951

#define REPEAT_START                  0x01
#define REPEAT_END                    0x02

#define WHOLE_NOTE                    0x03
#define HALF_NOTE                     0x04
#define QUARTER_NOTE                  0x05
#define EIGHTH_NOTE                   0x06
#define SIXTEENTH_NOTE                0x07
#define THIRTY_SECOND_NOTE            0x08

// Extends 0.5x
#define WHOLE_NOTE_DOTTED             0x09
#define HALF_NOTE_DOTTED              0x10
#define QUARTER_NOTE_DOTTED           0x11
#define EIGHTH_NOTE_DOTTED            0x12
#define SIXTEENTH_NOTE_DOTTED         0x13
#define THIRTY_SECOND_NOTE_DOTTED     0x14

// On same pitch
#define WHOLE_NOTE_TIE                0x15
#define HALF_NOTE_TIE                 0x16
#define QUARTER_NOTE_TIE              0x17
#define EIGHTH_NOTE_TIE               0x18
#define SIXTEENTH_NOTE_TIE            0x19
#define THIRTY_SECOND_NOTE_TIE        0x20

// On different pitch
#define WHOLE_NOTE_SLUR               0x21
#define HALF_NOTE_SLUR                0x22
#define QUARTER_NOTE_SLUR             0x23
#define EIGHTH_NOTE_SLUR              0x24
#define SIXTEENTH_NOTE_SLUR           0x25
#define THIRTY_SECOND_NOTE_SLUR       0x26
