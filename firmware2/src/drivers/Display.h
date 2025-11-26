#include <avr/io.h>
#include <Thread.h>

#ifndef WLDC_DISPLAY_DEBUG_MODE
#define WLDC_DISPLAY_DEBUG_MODE false
#endif

#ifndef WLDC_DISPLAY_DEBUG_WATCH_ALL
#define WLDC_DISPLAY_DEBUG_WATCH_ALL false
#endif

#ifndef WLDC_DISPLAY_SEGMENT_PORT
#define WLDC_DISPLAY_SEGMENT_PORT PORTD
#endif

#ifndef WLDC_DISPLAY_SEGMENT_DDR
#define WLDC_DISPLAY_SEGMENT_DDR DDRD
#endif

#ifndef WLDC_DISPLAY_DIGIT_PORT
#define WLDC_DISPLAY_DIGIT_PORT PORTB
#endif

#ifndef WLDC_DISPLAY_DIGIT_DDR
#define WLDC_DISPLAY_DIGIT_DDR DDRB
#endif

#ifndef WLDC_DISPLAY_DRIVER_H
#define WLDC_DISPLAY_DRIVER_H

#define DISP_LENGTH 4
// #define DISP_PIN_FIRST PB2
// #define DISP_PIN_LAST PB5
#define DISP_BR_MIN 1
#define DISP_BR_MAX 64
#define DISP_DEFAULT_SCROLL_INTERVAL 150
#define DISP_DEFAULT_FRACTION_DIGITS 2

#define GET_DIGIT_PIN_BIT(index) ({ const uint8_t map[DISP_LENGTH] = { PB2, PB3, PB4, PB5 }; map[index]; })


// Binary data
const byte seven_seg_ascii_init = ' '; // First mapped ASCII position

const byte seven_seg_asciis [] PROGMEM = {
  0b00000000,   // Space
  0x00, 0x00,   // Unmapped characters
  0x00, 0x00,   // Unmapped characters
  0x00, 0x00,   // Unmapped characters
  0x00, 0x00,   // Unmapped characters
  0x00,         // Unmapped characters
  
  0b11000110,   // * represents °
  0x00,         // Unmapped characters
  0b00001000,   // ,
  0b00000010,   // -
  0b00100000,   // .
  0b01001010,   // /
  
  // Numbers
  0b11111100,   // 0
  0b01100000,   // 1
  0b11011010,   // 2
  0b11110010,   // 3
  0b01100110,   // 4
  0b10110110,   // 5
  0b10111110,   // 6
  0b11100000,   // 7
  0b11111110,   // 8
  0b11110110,   // 9
  
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   // Unmapped characters
  
  // Alphabetic characters 
  0b11101110,   // A
  0b00111110,   // B
  0b10011100,   // C
  0b01111010,   // D
  0b10011110,   // E
  0b10001110,   // F
  0b11110110,   // G
  0b01101110,   // H
  0b00001100,   // I
  0b01111000,   // J
  0b00011110,   // K
  0b00011100,   // L
  0b10101010,   // M
  0b00101010,   // N
  0b00111010,   // O
  0b11001110,   // P
  0b11100110,   // Q
  0b00001010,   // R
  0b10110110,   // S
  0b01100010,   // T
  0b01111100,   // U
  0b00111000,   // V
  0b01010100,   // W
  0b10010010,   // X
  0b01110110,   // Y
  0b11011010,   // Z

  0b10011100,   // [
  0b00100110,   // `\`
  0b11110000,   // ]
  0b10000000,   // ^
  0b00010000,   // _
};


class DisplayDriver : public Thread {
protected:
  unsigned short brightness = DISP_BR_MAX / 2;

  byte content [DISP_LENGTH];
  unsigned short multiplex_count = 0;
  unsigned short multiplex_digit = 0;
  
  bool time_separator = false;
  unsigned short decimal_position = 0;
  unsigned short cursor = 0;

  char scroll_content[32];
  unsigned int scroll_cursor = 0;
  bool scroll_forward = true;

public:
  DisplayDriver();
  void begin();
  void run_multiplex();

  void setBrightness(unsigned short _brightness);
  void incrementBrightness();
  void decrementBrightness();
  void setTimeSeparator(bool _time_separator);

  unsigned short getBrightness();

  void setCursor(unsigned short _cursor);
  void clear();
  void print(char c);
  void print(char* c);
  void print(const __FlashStringHelper *);
  void print(int num);
  void print(unsigned short num);
  void print(double decimal);
  void print(double decimal, int fractionDigits);
  void printEnd(char c);
  void printEnd(char* c);
  void printEnd(const __FlashStringHelper *);
  void printEnd(int num);
  void printEnd(unsigned short num);
  void printEnd(double decimal);
  void printEnd(double decimal, int fractionDigits);
  void clearScroll();
  // void printScroll(char* text);
  // void printScroll(char* text, int _interval);
  void printScroll(const __FlashStringHelper *);
  void printScroll(const __FlashStringHelper *, int _interval);
  // void printScrollReverse(char* text);
  // void printScrollReverse(char* text, int _interval);
  void printScrollReverse(const __FlashStringHelper *);
  void printScrollReverse(const __FlashStringHelper *, int _interval);

  bool isScrolling();

  void run();
  
  void enable();
  void disable();
};

// Global display instance
extern DisplayDriver display;

#endif
