#ifndef WLDC_DISPLAY_DRIVER_H
#define WLDC_DISPLAY_DRIVER_H
#include <Arduino.h>
#include <Thread.h>

#define DISP_LENGTH 4
#define DISP_PIN_FIRST PB2
#define DISP_PIN_LAST PB5
#define DISP_BR_MIN 1
#define DISP_BR_MAX 64
#define DISP_SCROLL_INTERVAL 350

// Binary data
const byte seven_seg_ascii_init = ' '; // First mapped ASCII position
const byte seven_seg_ascii_end = 'Z'; // Last mapped ASCII position

const byte seven_seg_asciis [((int) seven_seg_ascii_end - seven_seg_ascii_init) + 1] = {
  0b00000000,   // Space
  0x00, 0x00,   // Unmapped characters
  0x00, 0x00,   // Unmapped characters
  0x00, 0x00,   // Unmapped characters
  0x00, 0x00,   // Unmapped characters
  0x00,         // Unmapped characters
  
  0b11000110,   // * represents °
  0x00, 0x00,   // Unmapped characters
  0b00000010,   // -
  0x00, 0x00,   // Unmapped characters
  
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
  0b11011010    // Z
};

class DisplayDriver : public Thread {
protected:
  int brightness = DISP_BR_MAX / 2;

  byte content [DISP_LENGTH];
  int multiplex_count = 0;
  int multiplex_digit = 0;
  
  bool time_separator = false;
  int decimal_position = 0;
  int cursor = 0;

  String scroll_content;
  int scroll_cursor = 0;
  bool scroll_forward = false;

public:
  DisplayDriver();
  void begin();
  void multiplex_run();

  void setBrightness(int _brightness);
  // void incrementBrightness();
  // void decrementBrightness();
  void setTimeSeparator(bool _time_separator);

  void setCursor(int _cursor);
  void clear();
  void print(char c);
  void print(const char* c);
  void print(int num);
  void print(double decimal);
  void print(double decimal, int fractionDigits);
  void print(String text);
  void printEnd(char c);
  void printEnd(const char* c);
  void printEnd(int num);
  void printEnd(double decimal);
  void printEnd(double decimal, int fractionDigits);
  void printEnd(String text);
  void printScroll(String text);
  void printScroll(String text, int _interval);
  void printScrollReverse(String text);
  void printScrollReverse(String text, int _interval);

  void run();
  
  void enable();
  void disable();
};

DisplayDriver::DisplayDriver(){
  enabled = false;
}

// Run sub-threads
void DisplayDriver::run(){
  clear();
  cursor = 0;

  // int startIndex = scroll_cursor - DISP_LENGTH;
  //const char* slice = scroll_content.substring(startIndex >= 0 ? startIndex : 0, scroll_cursor).c_str();
  const char* slice = scroll_content.substring(scroll_cursor, scroll_cursor + DISP_LENGTH).c_str();
  scroll_cursor++;

  for(int i = 0; i < strlen(slice) && cursor < DISP_LENGTH; i++){
    char c = toupper(slice[i]);
    content[cursor] = seven_seg_asciis[((int) c) - seven_seg_ascii_init];
    cursor++;
  }

  // Disable scroll and back to the main
  if(scroll_cursor > (scroll_content.length() - DISP_LENGTH + 1)){
    enabled = false;
  }

  Thread::run();
}

// bool DisplayDriver::shouldRun(unsigned long time){
//   return Thread::shouldRun(time);
// }


// Print methods
void DisplayDriver::setCursor(int col){
  cursor = col % DISP_LENGTH;
}

void DisplayDriver::clear(){
  for(int i = 0; i < DISP_LENGTH; i++)
    content[i] = 0x00;
}

void DisplayDriver::print(char c){
  decimal_position = 0;

  if(enabled) return;
  
  c = toupper(c);
  content[cursor] = seven_seg_asciis[((int) c) - seven_seg_ascii_init];
  cursor++;
}

void DisplayDriver::print(const char* c){
  for(int i = 0; i < strlen(c) && cursor < DISP_LENGTH; i++)
    print(c[i]);
}

void DisplayDriver::print(int num){
  char num_str [11] = "";
  itoa(num, num_str, 10);
  print(num_str);
}

void DisplayDriver::print(double decimal, int fractionDigits){
  int num = (int) decimal;
  int places = (int) ((decimal - num) * pow(10, fractionDigits));

  char num_str [11] = "";
  char places_str [11] = "";
  itoa(num, num_str, 10);
  itoa(places, places_str, 10);

  print(num_str);
  print(places_str);
  decimal_position = strlen(num_str);
}

void DisplayDriver::print(double decimal){
  print(decimal, 2);
}

void DisplayDriver::print(String text){
  print(text.c_str());
}

void DisplayDriver::printEnd(char c){
  setCursor(DISP_LENGTH - 1);
  print(c);
}

void DisplayDriver::printEnd(const char* c){
  setCursor(DISP_LENGTH - strlen(c));
  print(c);
}

void DisplayDriver::printEnd(int num){
  char num_str [11] = "";
  itoa(num, num_str, 10);
  printEnd(num_str);
}

void DisplayDriver::printEnd(double decimal, int fractionDigits){
  int num = (int) decimal;
  int places = (int) ((decimal - num) * pow(10, fractionDigits));

  char num_str [11] = "";
  char places_str [11] = "";
  itoa(num, num_str, 10);
  itoa(places, places_str, 10);

  setCursor(DISP_LENGTH - strlen(num_str) - strlen(places_str));
  print(num_str);
  print(places_str);

  decimal_position = DISP_LENGTH - strlen(places_str);
}

void DisplayDriver::printEnd(double decimal){
  printEnd(decimal, 2);
}

void DisplayDriver::printEnd(String text){
  printEnd(text.c_str());
}

void DisplayDriver::printScroll(String text, int _interval){
  time_separator = false;
  scroll_cursor = 0;
  scroll_content = text;
  scroll_forward = true;
  enabled = true;
  setInterval(_interval);

  Serial.println("printScroll");
}

void DisplayDriver::printScroll(String text){
  printScroll(text, DISP_SCROLL_INTERVAL);
}

void DisplayDriver::printScrollReverse(String text, int _interval){
  time_separator = false;
  scroll_cursor = 0;
  scroll_content = text;
  scroll_forward = false;
  enabled = true;
  setInterval(_interval);

  Serial.println("printScrollReverse");
}

void DisplayDriver::printScrollReverse(String text){
  printScrollReverse(text, DISP_SCROLL_INTERVAL);
}


// Getters and Setters
void DisplayDriver::setBrightness(int _brightness){
  if(_brightness > DISP_BR_MAX)
    brightness = DISP_BR_MAX;
  if(_brightness < DISP_BR_MIN)
    brightness = DISP_BR_MIN;
  else brightness = _brightness;
}

void DisplayDriver::setTimeSeparator(bool _time_separator){
  time_separator = _time_separator;
}


// Low level implementation
void DisplayDriver::begin(){
   // PinMode display digits pins
  DDRD = 0xff;                    // Set all PortD pins to output (0 - 7)
  PORTD = 0xff;                   // Set all PortD pins to HIGH

  // PinMode display select pins
  DDRB = B00111110;               // Set pins 9, 10, 11, 12, 13 to output and others to input
  PORTB = B00111110;              // Set all PortB pins to HIGH

  delay(100);
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

void DisplayDriver::multiplex_run(){
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
  Display.multiplex_run();
}

#endif
