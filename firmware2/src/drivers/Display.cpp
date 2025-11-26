#include "Display.h"

DisplayDriver display; 

DisplayDriver::DisplayDriver(){
  time_separator = false;
  scroll_cursor = 0;
  strcpy(scroll_content, "");
  scroll_forward = true;
  enabled = false;
  setInterval(DISP_DEFAULT_SCROLL_INTERVAL);
}

// Run sub-threads
void DisplayDriver::run(){
  time_separator = false;
  size_t scroll_content_length = strlen(scroll_content);
  
  if(scroll_cursor > (scroll_content_length - DISP_LENGTH)){
    enabled = false;
    strcpy(scroll_content, "");
    return Thread::run();
  }

  clear();
  setCursor(0);

  // Slice the text to the length of the screen with scroll offset
  char slice_str [DISP_LENGTH] = { 0 };
  const void * start_index = 0;

  if(scroll_forward)
    start_index = &scroll_content[0] + scroll_cursor;
  else 
    start_index = &scroll_content[0] + scroll_content_length - scroll_cursor - DISP_LENGTH;

  memcpy(slice_str, start_index, DISP_LENGTH);

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

  #if WLDC_DISPLAY_DEBUG_MODE && WLDC_DISPLAY_DEBUG_WATCH_ALL
    Serial.println((char*) content);
  #endif
}

void DisplayDriver::print(char c){
  
  c = toupper(c);

  #if WLDC_DISPLAY_DEBUG_MODE
    content[cursor] = c;

    if((WLDC_DISPLAY_DEBUG_WATCH_ALL || cursor == DISP_LENGTH -1)){
      for(size_t i = 0; i < DISP_LENGTH; i++) {
        Serial.print(content[i] == 0x00 ? ' ' : (char) content[i]);
        Serial.print(i == decimal_position -1 ? "." : " ");
        if(i == 1) Serial.print(time_separator ? ": " : "  ");
      }
      Serial.println();
    }
  #else
    content[cursor] = (char) pgm_read_word(&(seven_seg_asciis[((int) c) - seven_seg_ascii_init]));
  #endif
  
  decimal_position = 0;
  cursor++;
}

void DisplayDriver::print(char* c){
  for(size_t i = 0; i < strlen(c) && cursor < DISP_LENGTH; i++)
    print(c[i]);
}

void DisplayDriver::print(const __FlashStringHelper* text){
  char buffer[16];
  strncpy_P(buffer, (char*)text, 16);
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

void DisplayDriver::printEnd(char* c){
  setCursor(DISP_LENGTH - min(strlen(c), DISP_LENGTH));
  print(c);
}

void DisplayDriver::printEnd(const __FlashStringHelper* text){
  char buffer[16];
  strncpy_P(buffer, (char*)text, 16);
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
  strcpy(scroll_content, "");
  scroll_forward = true;
  enabled = false;
}

// void DisplayDriver::printScroll(char* text, int _interval){
//   time_separator = false;
//   decimal_position = 0;
//   scroll_cursor = 0;
//   strcpy(scroll_content, text);
//   scroll_forward = true;
//   enabled = true;
//   setInterval(_interval);
// }

// void DisplayDriver::printScroll(char* text){
//   printScroll(text, DISP_DEFAULT_SCROLL_INTERVAL);
// }

void DisplayDriver::printScroll(const __FlashStringHelper* text, int _interval) {
  setTimeSeparator(false);
  decimal_position = 0;
  scroll_cursor = 0;
  scroll_forward = true;
  enabled = true;
  setInterval(_interval);

  const char* flashPtr = reinterpret_cast<const char*>(text);
  strncpy_P(scroll_content, flashPtr, 32);
}

void DisplayDriver::printScroll(const __FlashStringHelper * text){
  printScroll(text, DISP_DEFAULT_SCROLL_INTERVAL);
}

// void DisplayDriver::printScrollReverse(char* text, int _interval){
//   setTimeSeparator(false);
//   time_separator = false;
//   decimal_position = 0;
//   scroll_cursor = 0;
//   strcpy(scroll_content, text);
//   scroll_forward = false;
//   enabled = true;
//   setInterval(_interval);
// }

// void DisplayDriver::printScrollReverse(char* text){
//   printScrollReverse(text, DISP_DEFAULT_SCROLL_INTERVAL);
// }

void DisplayDriver::printScrollReverse(const __FlashStringHelper * text, int _interval){
  time_separator = false;
  decimal_position = 0;
  scroll_cursor = 0;
  strncpy_P(scroll_content, (char*)text, 256);
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
  // Serial.print("DisplayDriver::setTimeSeparator ");
  // Serial.println(_time_separator ? "true" : "false");
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
  #if WLDC_DISPLAY_DEBUG_MODE
    Serial.begin(9600);
    return;
  #endif

   // PinMode display digits pins
  WLDC_DISPLAY_SEGMENT_DDR = 0xff;                    // Set all PortD pins to output (0 - 7)
  WLDC_DISPLAY_SEGMENT_PORT = 0xff;                   // Set all PortD pins to HIGH

  // PinMode display select pins
  WLDC_DISPLAY_DIGIT_DDR = B00111110;               // Set pins 9, 10, 11, 12, 13 to output and others to input
  WLDC_DISPLAY_DIGIT_PORT = B00111110;              // Set all PortB pins to HIGH

  delay(50);
  WLDC_DISPLAY_DIGIT_PORT = B00111100;
  delay(50);
  WLDC_DISPLAY_DIGIT_PORT = B00111110;
  delay(100);

  WLDC_DISPLAY_SEGMENT_PORT = 0x00;                   // Set all PortD pins to LOW 
  WLDC_DISPLAY_DIGIT_PORT = 0x00;                   // Set all PortB pins to LOW

  /*    *    ATMEGA TIMER2    *    */
  
  // Turn on CTC mode
  TCCR2A = 0x00;
  TCCR2A |= (1 << WGM21);

  // Set CS21 bit for 8 pre-scaler
  TCCR2B = 0x00;
  TCCR2B |= (1 << CS21);

  // Initialize Registers
  TCNT2  = 0;
  OCR2A = 100;

  if(WLDC_DISPLAY_DEBUG_MODE) {
    disable();
  }
  else {
    enable();
  }
}

void DisplayDriver::enable(){
  if(WLDC_DISPLAY_DEBUG_MODE) return;
  TIMSK2 |= (1 << OCIE2A); // enable timer compare interrupt
}

void DisplayDriver::disable(){
  TIMSK2 &= ~(1 << OCIE2A); // disable timer compare interrupt
  WLDC_DISPLAY_SEGMENT_PORT = 0x00;             // Clean display pins
}

void DisplayDriver::run_multiplex(){
  // Clean display
  WLDC_DISPLAY_SEGMENT_PORT = 0x00;                                   // Sets all PortD pins to LOW

  // Brightness control
  if((multiplex_count/DISP_LENGTH) < brightness){
    WLDC_DISPLAY_DIGIT_PORT |= 0b00111100;                          // Puts pins PB2, PB3, PB4, PB5 to HIGH
    WLDC_DISPLAY_DIGIT_PORT ^= 0b00111100;                          // Toggle pins PB2, PB3, PB4, PB5 to LOW
  
    // Active the current digit of display
    WLDC_DISPLAY_DIGIT_PORT ^= (1 << GET_DIGIT_PIN_BIT(multiplex_digit));  // Toggle the current digit pin to HIGH
  
    // Set display content
    WLDC_DISPLAY_SEGMENT_PORT ^= content[multiplex_digit];                 // Sets the display content and uses ^= to invert the bits
                                                  // (the display actives with LOW state)
    
    // Set time marker
    if(time_separator && multiplex_digit == DISP_LENGTH -1)
      WLDC_DISPLAY_SEGMENT_PORT ^= (1 << 0);

    // Set decimal position
    if(decimal_position == multiplex_digit + 1 && decimal_position <= 3) {
      WLDC_DISPLAY_SEGMENT_PORT ^= (1 << 0);
    }
  }

  // Increment the current digit register
  multiplex_digit++;
  multiplex_digit %= DISP_LENGTH;

  // Increment the brightness loop counter
  multiplex_count++;
  multiplex_count %= DISP_BR_MAX * DISP_LENGTH;
}

// TIMER2 Interrupt
ISR(TIMER2_COMPA_vect){
  display.run_multiplex();
}
