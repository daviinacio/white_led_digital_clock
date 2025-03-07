#include "Arduino.h"
#ifndef WLDC_DISPLAY_DRIVER_H
#define WLDC_DISPLAY_DRIVER_H
#include <Thread.h>

#define DISP_LENGTH 4
#define DISP_PIN_FIRST PB2
#define DISP_PIN_LAST PB5
// #define DISP_BR_MIN 1
// #define DISP_BR_MAX 64
#define DISP_DEFAULT_SCROLL_INTERVAL 200
#define DISP_DEFAULT_FRACTION_DIGITS 2

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
  
  // Alfabetic characters 
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

  const char* scroll_content;
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
  void print(const char* c);
  void print(const __FlashStringHelper *);
  void print(int num);
  void print(unsigned short num);
  void print(double decimal);
  void print(double decimal, int fractionDigits);
  void printEnd(char c);
  void printEnd(const char* c);
  void printEnd(const __FlashStringHelper *);
  void printEnd(int num);
  void printEnd(unsigned short num);
  void printEnd(double decimal);
  void printEnd(double decimal, int fractionDigits);
  void clearScroll();
  void printScroll(const char* text);
  void printScroll(const char* text, int _interval);
  void printScroll(const __FlashStringHelper *);
  void printScroll(const __FlashStringHelper *, int _interval);
  void printScrollReverse(const char* text);
  void printScrollReverse(const char* text, int _interval);
  void printScrollReverse(const __FlashStringHelper *);
  void printScrollReverse(const __FlashStringHelper *, int _interval);

  bool isScrolling();

  void run();
  
  void enable();
  void disable();
};

DisplayDriver::DisplayDriver(){
  time_separator = false;
  scroll_cursor = 0;
  scroll_content = "";
  scroll_forward = true;
  enabled = false;
  setInterval(DISP_DEFAULT_SCROLL_INTERVAL);
}

// Run sub-threads
void DisplayDriver::run(){
  size_t scroll_content_length = strlen(scroll_content);
  
  if(scroll_cursor > (scroll_content_length - DISP_LENGTH)){
    enabled = false;
    scroll_cursor++;
    scroll_content = "";
    return;
  }

  clear();
  setCursor(0);

  // Slice the text to the length of the screen with scroll offset
  char slice_str [DISP_LENGTH] = { 0 };
  if(scroll_forward)
    memcpy(slice_str, &scroll_content[0] + scroll_cursor, DISP_LENGTH);
  else 
    memcpy(slice_str, &scroll_content[0] + scroll_content_length - scroll_cursor - DISP_LENGTH, DISP_LENGTH);
  slice_str[DISP_LENGTH] = 0;

  scroll_cursor++;
  print(slice_str);

  Thread::run();
}


// Print methods
void DisplayDriver::setCursor(unsigned short col){
  cursor = col % DISP_LENGTH;
}

void DisplayDriver::clear(){
  for(int i = 0; i < DISP_LENGTH; i++)
    content[i] = 0x00;
}

void DisplayDriver::print(char c){
  decimal_position = 0;
  
  c = toupper(c);
  content[cursor] = (char) pgm_read_word(&(seven_seg_asciis[((int) c) - seven_seg_ascii_init]));
  cursor++;
}

void DisplayDriver::print(const char* c){
  for(int i = 0; i < strlen(c) && cursor < DISP_LENGTH; i++)
    print(c[i]);
}

void DisplayDriver::print(const __FlashStringHelper* text){
  char buffer[16];
  strncpy_P(buffer, (const char*)text, 16);
  print(buffer);
}

void DisplayDriver::print(int num){
  char num_str [11] = "";
  itoa(num, num_str, 10);
  print(num_str);
}

void DisplayDriver::print(unsigned short num){
  print((int) num);
}

void DisplayDriver::print(double decimal, int fractionDigits){
  int num = (int) decimal;
  unsigned int places = round((decimal - num) * pow(10, fractionDigits));
  // unsigned short places = modf(decimal);
  // unsigned short places = round(fmod(decimal, 1) * pow(10, fractionDigits));

  char num_str [11] = "";
  char places_str [11] = "";
  itoa(num, num_str, 10);
  itoa(places, places_str, 10);

  if(!enabled)
    decimal_position = cursor + strlen(num_str);

  print(num_str);
  print(places_str);
}

void DisplayDriver::print(double decimal){
  print(decimal, DISP_DEFAULT_FRACTION_DIGITS);
}

void DisplayDriver::printEnd(char c){
  setCursor(DISP_LENGTH - 1);
  print(c);
}

void DisplayDriver::printEnd(const char* c){
  setCursor(DISP_LENGTH - min(strlen(c), DISP_LENGTH));
  print(c);
}

void DisplayDriver::printEnd(const __FlashStringHelper* text){
  char buffer[16];
  strncpy_P(buffer, (const char*)text, 16);
  printEnd(buffer);
}

void DisplayDriver::printEnd(int num){
  char num_str [11] = "";
  itoa(num, num_str, 10);
  printEnd(num_str);
}

void DisplayDriver::printEnd(unsigned short num){
  printEnd((int) num);
}

void DisplayDriver::printEnd(double decimal, int fractionDigits){
  int num = (int) decimal;
  unsigned int places = round((decimal - num) * pow(10, fractionDigits));

  char num_str [11] = "";
  char places_str [11] = "";
  itoa(num, num_str, 10);
  itoa(places, places_str, 10);

  setCursor(DISP_LENGTH - strlen(num_str) - strlen(places_str));
  print(num_str);
  print(places_str);

  if(!enabled)
    decimal_position = DISP_LENGTH - strlen(places_str);
}

void DisplayDriver::printEnd(double decimal){
  printEnd(decimal, DISP_DEFAULT_FRACTION_DIGITS);
}

void DisplayDriver::clearScroll(){
  scroll_cursor = 0;
  scroll_content = "";
  scroll_forward = true;
  enabled = false;
}

void DisplayDriver::printScroll(const char* text, int _interval){
  time_separator = false;
  decimal_position = 0;
  scroll_cursor = 0;
  scroll_content = text;
  scroll_forward = true;
  enabled = true;
  setInterval(_interval);
}

void DisplayDriver::printScroll(const char* text){
  printScroll(text, DISP_DEFAULT_SCROLL_INTERVAL);
}

void DisplayDriver::printScroll(const __FlashStringHelper * text, int _interval){
  time_separator = false;
  decimal_position = 0;
  scroll_cursor = 0;
  strncpy_P(scroll_content, (const char*)text, 32);
  scroll_forward = true;
  enabled = true;
  setInterval(_interval);
}

void DisplayDriver::printScroll(const __FlashStringHelper * text){
  printScroll(text, DISP_DEFAULT_SCROLL_INTERVAL);
}

void DisplayDriver::printScrollReverse(const char* text, int _interval){
  time_separator = false;
  decimal_position = 0;
  scroll_cursor = 0;
  scroll_content = text;
  scroll_forward = false;
  enabled = true;
  setInterval(_interval);
}

void DisplayDriver::printScrollReverse(const char* text){
  printScrollReverse(text, DISP_DEFAULT_SCROLL_INTERVAL);
}

void DisplayDriver::printScrollReverse(const __FlashStringHelper * text, int _interval){
  time_separator = false;
  decimal_position = 0;
  scroll_cursor = 0;
  strncpy_P(scroll_content, (const char*)text, 256);
  scroll_forward = false;
  enabled = true;
  setInterval(_interval);
}

void DisplayDriver::printScrollReverse(const __FlashStringHelper * text) {
 printScrollReverse(text, DISP_DEFAULT_SCROLL_INTERVAL);
}


// Getters and Setters
void DisplayDriver::setBrightness(unsigned short _brightness){
  if(_brightness > DISP_BR_MAX)
    brightness = DISP_BR_MAX;
  else if(_brightness <= DISP_BR_MIN)
    brightness = DISP_BR_MIN;
  else brightness = _brightness;
}

void DisplayDriver::incrementBrightness(){
  int temp = brightness;
  temp += DISP_BR_MAX/8;
  if(temp % 2 != 0) temp -= 1;
  setBrightness(temp);
}

void DisplayDriver::decrementBrightness(){
  int temp =  brightness;
  temp -= DISP_BR_MAX/8;
  setBrightness(max(temp, DISP_BR_MIN));
}

void DisplayDriver::setTimeSeparator(bool _time_separator){
  time_separator = _time_separator;
}

unsigned short DisplayDriver::getBrightness(){
  return brightness;
}

bool DisplayDriver::isScrolling(){
  return enabled;
}


// Low level implementation
void DisplayDriver::begin(){
   // PinMode display digits pins
  DDRD = 0xff;                    // Set all PortD pins to output (0 - 7)
  PORTD = 0xff;                   // Set all PortD pins to HIGH

  // PinMode display select pins
  DDRB = B00111110;               // Set pins 9, 10, 11, 12, 13 to output and others to input
  PORTB = B00111110;              // Set all PortB pins to HIGH

  delay(50);
  PORTB = B00111100;
  delay(50);
  PORTB = B00111110;
  delay(100);

  PORTD = 0x00;                   // Set all PortD pins to LOW 
  PORTB = 0x00;                   // Set all PortB pins to LOW

  /*    *    ATMEGA TIMER2    *    */
  
  // Turn on CTC mode
  TCCR2A = 0x00;
  TCCR2A |= (1 << WGM21);

  // Set CS21 bit for 8 prescaler
  TCCR2B = 0x00;
  TCCR2B |= (1 << CS21);

  // Initialize Registers
  TCNT2  = 0;
  OCR2A = 100;
}

void DisplayDriver::enable(){
  TIMSK2 |= (1 << OCIE2A); // enable timer compare interrupt
}

void DisplayDriver::disable(){
  TIMSK2 &= ~(1 << OCIE2A); // disable timer compare interrupt
  PORTD = 0x00;             // Clean display pins
}

void DisplayDriver::run_multiplex(){
  // Clean display
  PORTD = 0x00;                                   // Sets all PORTD pins to LOW

  // Brightness control
  if((multiplex_count/DISP_LENGTH) < brightness){
    PORTB |= 0b00111100;                          // Puts pins PB2, PB3, PB4, PB5 to HIGH
    PORTB ^= 0b00111100;                          // Toggle pins PB2, PB3, PB4, PB5 to LOW
  
    // Active the current digit of display
    PORTB ^= (1 << DISP_PIN_FIRST + multiplex_digit);  // Toggle the current digit pin to HIGH
  
    // Set display content
    PORTD ^= content[multiplex_digit];                 // Sets the display content and uses ^= to invert the bits
                                                  // (the display actives with LOW state)
    
    // Set time marker
    if(time_separator && multiplex_digit == DISP_LENGTH -1)
      PORTD ^= (1 << 0);

    // Set decimal position
    if(decimal_position == multiplex_digit + 1 && decimal_position <= 3) {
      PORTD ^= (1 << 0);
    }
  }

  // Increment the current digit register
  multiplex_digit++;
  multiplex_digit %= DISP_LENGTH;

  // Increment the brightness loop counter
  multiplex_count++;
  multiplex_count %= DISP_BR_MAX * DISP_LENGTH;
}

// Global display instance
DisplayDriver Display = DisplayDriver();

// TIMER2 Interrupt
ISR(TIMER2_COMPA_vect){
  Display.run_multiplex();
}

#endif
