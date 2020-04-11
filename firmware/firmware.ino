/*
 * (c) daviapps 2020
 * 
 * White LED Digital Clock 
 * 
 * description: This is the firmware of a led digital clock (7 segments)
 * author: github.com/daviinacio
 * date: 16/01/2019
 * 
 */

#define LDR_PIN A2

#define DISP_BR_MAX 64
#define DISP_BR_MIN 1
#define DISP_BR_BUFFER_LENGTH 16
#define DISP_BR_BUFFER_INTERVAL 2000

#define DISP_LENGTH 4
#define DISP_PIN_FIRST PB2
#define DISP_PIN_LAST PB5

#define MAIN_INTERVAL 100
#define MAIN_SCREEN_HOME 1000
#define MAIN_SCREEN_LDR 1001
#define MAIN_SCREEN_CHRONOMETER 1002
#define MAIN_SCREEN_REMOTE 1003
#define MAIN_SCREEN_TIME_ADJUST 1004

#define CHRONOMETER_INTERVAL 1000

#define RTC_INTERVAL 1000

#define DHT_INIT_VALUE -255
#define DHT_PIN A0
#define DHT_TYPE DHT::DHT11
#define DHT_BUFFER_LENGTH 4
#define DHT_BUFFER_INTERVAL 60000

#define IR_PIN A1
#define IR_INTERVAL 250

#define BZ_PIN PB1
#define BZ_INTERVAL 62
#define BZ_STATUS_OFF 1100
#define BZ_STATUS_OLDCLOCK 1101

#define SCROLL_INTERVAL 350

#define ADJUST_CURSOR_RANGE 5

//#define RTC_FIX_INTERVAL 150000   // 150s or 2.5m
//#define RTC_FIX_INTERVAL 180000   // 180s or 3m
#define RTC_FIX_INTERVAL 662712   // 180s or 11m
#define RTC_FIX_OPERATION --

// Binary data
const byte seven_seg_ascii_init = ' '; // First mapped ascci position
const byte seven_seg_ascii_end  = 'Z'; // Last mapped ascci position

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
  0b00111000,   // U
  0b01111100,   // V
  0b01010100,   // W
  0b10010010,   // X
  0b01110110,   // Y
  0b11011010    // Z
};

// Library imports
#include <Thread.h>
#include <ThreadController.h>

#include <Wire.h>
#include "RTClib.h"

#include "DHT.h"

#include <IRremote.h>

#include <Buffer.h>

// Threads
ThreadController cpu = ThreadController();
Thread thr_main = Thread();
Thread thr_chronometer = Thread();

Thread thr_ldr = Thread();
Thread thr_rtc = Thread();
Thread thr_dht = Thread();
Thread thr_ir = Thread();
Thread thr_buzzer = Thread();
Thread thr_scroll = Thread();
Thread thr_rtc_fix = Thread();
//Thread thr_panel = Thread();

// Display
byte disp_content [DISP_LENGTH];

Buffer disp_brightness_buffer(DISP_BR_BUFFER_LENGTH, DISP_BR_MAX);

int disp_count = 0; 
int disp_digit = 0;

int disp_content_cursor = 0;

// RTC
RTC_DS1307 rtc;
DateTime rtc_now;

// DHT
DHT dht;

Buffer dht_temp_buffer(DHT_BUFFER_LENGTH, DHT_INIT_VALUE);
Buffer dht_hum_buffer(DHT_BUFFER_LENGTH, DHT_INIT_VALUE);

// IR Remore
IRrecv ir_recv(IR_PIN);
decode_results ir_results;

// Main
int main_current_screen = MAIN_SCREEN_HOME;
bool main_change_loop = true;

// Chronometer
int chronometer_counter = 0;

// Buzzer
int buzzer_status = BZ_STATUS_OFF;

// Scroll
String disp_scroll_buffer;
int disp_scroll_cursor_init = 0;
bool disp_scroll_dir = false; // true: right | false: left

// Time Adjust
int time_adjust_cursor = 0;

int time_adjust_year = 0;
int time_adjust_month = 0;
int time_adjust_day = 0;
int time_adjust_hour = 0;
int time_adjust_minute = 0;
int time_adjust_second = 0;

bool time_adjust_cursor_blink = false;

// DEBUG
int debug_thread_loop_tester = 0;

void setup() {
  /*    *    *    PIN MODE   *    *    */
  
  // PinMode display digits pins
  DDRD = 0xff;                    // Set all PortD pins to output (0 - 7)
  PORTD = 0xff;                   // Set all PortD pins to HIGH

  // PinMode display select pins
  DDRB = B00111110;               // Set pins 9, 10, 11, 12, 13 to output and others to input
  PORTB = B00111110;              // Set all PortB pins to HIGH

  delay(500);

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
  
  // Enable to TIMER2 Interrupt
  TIMSK2 |= (1 << OCIE2A);

  /*    *    *  THREADS  *    *    */

  // Initialize threads
  thr_main.onRun(thr_main_func);
  thr_main.setInterval(MAIN_INTERVAL);

  thr_chronometer.onRun(thr_chronometer_func);
  thr_chronometer.setInterval(CHRONOMETER_INTERVAL);
  thr_chronometer.enabled = false;

  thr_ldr.onRun(thr_ldr_func);
  thr_ldr.setInterval(DISP_BR_BUFFER_INTERVAL / DISP_BR_BUFFER_LENGTH);

  thr_rtc.onRun(thr_rtc_func);
  thr_rtc.setInterval(RTC_INTERVAL);

  thr_dht.onRun(thr_dht_func);
  thr_dht.setInterval(DHT_BUFFER_INTERVAL / DHT_BUFFER_LENGTH);

  thr_ir.onRun(thr_ir_func);
  thr_ir.setInterval(IR_INTERVAL);

  thr_buzzer.onRun(thr_buzzer_func);
  thr_buzzer.setInterval(BZ_INTERVAL);

  thr_scroll.onRun(thr_scroll_func);
  thr_scroll.setInterval(SCROLL_INTERVAL);

  thr_rtc_fix.onRun(thr_rtc_fix_func);
  thr_rtc_fix.setInterval(RTC_FIX_INTERVAL);

  //thr_panel.onRun(thread_panel_loop);
  //thr_panel.setInterval(1000);


  // Add thread to thead controll
  cpu.add(&thr_main);
  cpu.add(&thr_chronometer);
  cpu.add(&thr_ldr);
  cpu.add(&thr_rtc);
  cpu.add(&thr_dht);
  cpu.add(&thr_ir);
  cpu.add(&thr_buzzer);
  cpu.add(&thr_scroll);
  cpu.add(&thr_rtc_fix);
  
  /*    *  LIBRARY BEGINNERS  *    */

  Wire.begin();
  dht.setup(DHT_PIN, DHT_TYPE);
  ir_recv.enableIRIn();

  // Wait for RTC begin
  while(!rtc.begin()){
    // Blink error for five seconds
    if(millis()/500 % 3 >= 1){
      disp_setCursor(0);
      disp_print((char*)"ERRO");
    } else {
      disp_clear();
    }
  }

  /*    *   CHECK COMPONENTS  *    */

  if (!rtc.isrunning()) {
    // Reset time
    rtc.adjust(DateTime(__DATE__, __TIME__));

    // Goto time adjust screen
    main_current_screen = MAIN_SCREEN_TIME_ADJUST;
    time_adjust_cursor = 1;
    thr_rtc.enabled = false;
    thr_rtc_fix.enabled = false;
  }

  /*    *   THREAD FIRST RUN  *    */
  
  thr_ldr_func();
  thr_rtc_func();
  thr_dht_func();

  /*    *     END OF BOOT     *    */

  thr_scroll.enabled = false;

  disp_clear();
  disp_setCursor(0);
  disp_print((char*)"DAVI");

  // DEBUG
  //thr_ldr.enabled = false;
  //disp_brightness_buffer.fill(DISP_BR_MAX);
}

void loop() {
  // Run the main thread controller
  cpu.run();
}

/*    *    *    *    THREAD    *    *    *    */

void thr_main_func() {
  long m = millis();
  
  switch(main_current_screen){
    case MAIN_SCREEN_HOME:
      thr_main.setInterval(MAIN_INTERVAL);
    
      // Initial interval
      if(m <= 2000){ /* Do nothing */ } else
      
      // Temperature                  // Each 20s, runs on 16s, per 2s
      if(m / 2000 % 10 == 8 && dht_temp_buffer.getAverage() != DHT_INIT_VALUE && main_change_loop){
        disp_setCursor(0);
        disp_print((int) dht_temp_buffer.getAverage());
        disp_print((char*)"*C");
      } else
      
      // Humidity                     // Each 20s, runs on 18s, per 2s
      if(m / 2000 % 10 == 9 && dht_hum_buffer.getAverage() != DHT_INIT_VALUE && main_change_loop){
        disp_setCursor(0);
        disp_print((char*)"H ");
        disp_printEnd((int) dht_hum_buffer.getAverage());
      }
      
      // Hours and Minutes
      else {                          // Runs when others 'IFs' are false
        disp_setCursor(0);
        
        if(rtc_now.hour() < 10)
          disp_print(0);
        disp_print(rtc_now.hour());

        if(rtc_now.minute() < 10)
          disp_print(0);
        disp_print(rtc_now.minute());
      }
      break;

    case MAIN_SCREEN_LDR:
      disp_setCursor(0);
      disp_print((char*)"BR");

      if((int) disp_brightness_buffer.getAverage() < 10)
        disp_print((char*)" ");
      
      disp_printEnd((int) disp_brightness_buffer.getAverage());
      break;

    case MAIN_SCREEN_CHRONOMETER:
      thr_main.setInterval(CHRONOMETER_INTERVAL);
      disp_clear();
      disp_printEnd(chronometer_counter);
      break;

    case MAIN_SCREEN_TIME_ADJUST:
      disp_setCursor(0);
    
      if(time_adjust_cursor == 0 || time_adjust_cursor == 1){  // Minutes & Hours
        disp_setCursor(0);

        if((time_adjust_cursor == 1 && millis()/500 % 3 == 0) && time_adjust_cursor_blink){   // Blink 1/3 on focus
          disp_print((char*)"  ");
        }
        else {
          if(time_adjust_hour < 10)
            disp_print(0);
          disp_print(time_adjust_hour);
        }

        if((time_adjust_cursor == 0 && millis()/500 % 3 == 0) && time_adjust_cursor_blink){   // Blink 1/3 on focus
          disp_print((char*)"  ");
        }
        else {
          if(time_adjust_minute < 10)
            disp_print(0);
          disp_print(time_adjust_minute);
        }
      }
      else
      if(time_adjust_cursor == 2){  // Day
        disp_setCursor(0);
        disp_print((char*)"D ");

        if((millis()/500 % 3 == 0) && time_adjust_cursor_blink){  // Blink 1/3 on focus
          disp_print((char*)"  ");
        }
        else {
          if(time_adjust_day % 100 < 10)
            disp_print((char*)" ");
            
          disp_printEnd(time_adjust_day);
        }
      }
      else
      if(time_adjust_cursor == 3){  // Month
        disp_setCursor(0);
        disp_print((char*)"M ");

        if((millis()/500 % 3 == 0) && time_adjust_cursor_blink){  // Blink 1/3 on focus
          disp_print((char*)"  ");
        }
        else {
          if(time_adjust_month % 100 < 10)
            disp_print((char*)" ");
            
          disp_printEnd(time_adjust_month);
        }
      }
      else
      if(time_adjust_cursor == 4){  // Year
        disp_setCursor(0);
        disp_print((char*)"Y ");

        if((millis()/500 % 3 == 0) && time_adjust_cursor_blink){  // Blink 1/3 on focus
          disp_print((char*)"  ");
        }
        else {
          if(time_adjust_year % 100 < 10)
            disp_print((char*)" ");
            
          disp_printEnd(time_adjust_year % 100);
        }
      }
      break;

    // @deprecated
    case MAIN_SCREEN_REMOTE:
      unsigned long value = ir_results.value;
      disp_scroll("  0x" + String(value, 16) + "  " , 1000);
      break;

    default:
      disp_clear();
      disp_setCursor(0);
      disp_print((char*)"MERR");
      break;
  }
}

void thr_chronometer_func(){
  chronometer_counter++;
  chronometer_counter %= (int) pow(10, DISP_LENGTH);
}

void thr_ldr_func(){
  // Make the ldr sensor read
  int br_read = map(analogRead(LDR_PIN), 0, 650, DISP_BR_MIN, DISP_BR_MAX);

  // Avoid out of range problems
  if(br_read > DISP_BR_MAX) br_read = DISP_BR_MAX; else
  if(br_read < DISP_BR_MIN) br_read = DISP_BR_MIN;

  disp_brightness_buffer.insert(br_read);
}

void thr_rtc_func(){
  rtc_now = rtc.now();
}

void thr_dht_func(){
  // Disable display timer while dht read
  DispTimer_disable();
  float dht_temp_read = dht.getTemperature();
  float dht_hum_read = dht.getHumidity();
  DispTimer_enable();

  // Ignore read errors
  if(dht.getStatus() != DHT::ERROR_NONE) return;

  // Avoid first read problems
  if(dht_temp_buffer.empty()) dht_temp_buffer.fill((int) dht_temp_read);
  else                        dht_temp_buffer.insert((int) dht_temp_read);

  if(dht_hum_buffer.empty())  dht_hum_buffer.fill((int) dht_hum_read);
  else                        dht_hum_buffer.insert((int) dht_hum_read);
}

void thr_ir_func(){
  //for (int read_try = 0; ir_recv.decode(&ir_results) and read_try < IR_TRYOUT; read_try++) {
  if(ir_recv.decode(&ir_results)){
    // Read the ir HEX value
    unsigned long value = ir_results.value;
    // Receive the next value
    ir_recv.resume();

    // Temporary variable
    int tp;

    switch(value){
      case 0xC26BF044: // Up
        if(main_current_screen == MAIN_SCREEN_HOME)
          buzzer_status = BZ_STATUS_OLDCLOCK;
        else
        if(main_current_screen == MAIN_SCREEN_TIME_ADJUST){
          time_adjust_cursor_blink = false;
          switch(time_adjust_cursor){
            case 0: if(time_adjust_minute < 59) time_adjust_minute++; break;
            case 1: if(time_adjust_hour < 23) time_adjust_hour++; break;
            case 2: if(time_adjust_day < 31) time_adjust_day++; break;
            case 3: if(time_adjust_month < 12) time_adjust_month++; break;
            case 4: if(time_adjust_year < 2100) time_adjust_year++; break;
          }
        }
        break;

      case 0xC4FFB646: // Down
        if(main_current_screen == MAIN_SCREEN_HOME)
          dht_temp_buffer.fill(-42);
        else
        if(main_current_screen == MAIN_SCREEN_TIME_ADJUST){
          time_adjust_cursor_blink = false;
          switch(time_adjust_cursor){
            case 0: if(time_adjust_minute > 0) time_adjust_minute--; break;
            case 1: if(time_adjust_hour > 0) time_adjust_hour--; break;
            case 2: if(time_adjust_day > 0) time_adjust_day--; break;
            case 3: if(time_adjust_month > 0) time_adjust_month--; break;
            case 4: if(time_adjust_year > 2000) time_adjust_year--; break;
          }
        }
        break; 

      case 0x758C9D82: // Left
        if(main_current_screen == MAIN_SCREEN_TIME_ADJUST){
          time_adjust_cursor_blink = false;
          if(time_adjust_cursor < ADJUST_CURSOR_RANGE - 1)
            time_adjust_cursor++;
        }
        break;

      case 0x53801EE8: // Right
        if(main_current_screen == MAIN_SCREEN_TIME_ADJUST){
          time_adjust_cursor_blink = false;
          if(time_adjust_cursor > 0)
            time_adjust_cursor--;
        }
        break;

      case 0x8AF13528:  // Center
        if(main_current_screen == MAIN_SCREEN_HOME)
          buzzer_status = BZ_STATUS_OFF; 
        break;
      
      case 0x3BCD58C8: // Return
      case 0x974F362:  // Exit
        if(main_current_screen == MAIN_SCREEN_TIME_ADJUST){
          rtc.adjust(DateTime(
            time_adjust_year, time_adjust_month, time_adjust_day,
            time_adjust_hour, time_adjust_minute, 1
          ));
          disp_scroll("SAVE", 1500);
        }

        main_current_screen = MAIN_SCREEN_HOME;
        thr_rtc.enabled = true;
        thr_rtc_fix.enabled = true;
        break;
      
      case 0x68733A46:  //disp_brightness += DISP_BR_MAX/8; thr_ldr.enabled = false; 
        thr_ldr.enabled = false;
        
        tp = disp_brightness_buffer.getAverage();
        tp += DISP_BR_MAX/8;

        if(tp % 2 != 0) tp -= 1;
        if(tp > DISP_BR_MAX) tp = DISP_BR_MAX;

        disp_brightness_buffer.fill(tp);
        break; // Vol+
        
      case 0x83B19366:  //disp_brightness -= DISP_BR_MAX/8; thr_ldr.enabled = false;
        thr_ldr.enabled = false;
        
        tp = disp_brightness_buffer.getAverage();
        tp -= DISP_BR_MAX/8;
        if(tp < DISP_BR_MIN) tp = DISP_BR_MIN;

        disp_brightness_buffer.fill(tp);
        break; // Vol-
        
      case 0x2340B922:
        if(!thr_ldr.enabled)
          disp_scroll("AUTO", 1500);
        thr_ldr.enabled = true; break;
        // Mute

      case 0XCE3693E6:  disp_scroll("   DESENVOLVIDO POR DAVI INACIO    ", 300); break; // Content
      case 0x6BDD79E6:  disp_scroll("    - FELIZ DIA DO PROGRAMADOR -    ", 250); break; // Content
  
      case 0xDAEA83EC:
        if(main_current_screen != MAIN_SCREEN_LDR)
          disp_scroll("   BRILHO   ");
          
        main_current_screen = MAIN_SCREEN_LDR;
      break; // A
      
      case 0x2BAFCEEC:
        if(main_current_screen != MAIN_SCREEN_CHRONOMETER)
          disp_scroll("   CRONOMETRO   ");
        main_current_screen = MAIN_SCREEN_CHRONOMETER;
        break; // B
      case 0xB5210DA6:
        //if(main_current_screen != MAIN_SCREEN_REMOTE)
        //  disp_scroll("   CONTROLE HEX   ");
        //main_current_screen = MAIN_SCREEN_REMOTE;
        break; // C
      case 0x71A1FE88:
        if(main_current_screen != MAIN_SCREEN_TIME_ADJUST)
          disp_scroll("   SET TIME   ");

        if (rtc.isrunning()) {
          rtc_now = rtc.now();
          time_adjust_year = rtc_now.year();
          time_adjust_month = rtc_now.month();
          time_adjust_day = rtc_now.day();
          time_adjust_hour = rtc_now.hour();
          time_adjust_minute = rtc_now.minute();
        }
        
        thr_rtc.enabled = false;
        thr_rtc_fix.enabled = false;
        time_adjust_cursor_blink = true;
        main_current_screen = MAIN_SCREEN_TIME_ADJUST;
        time_adjust_cursor = 1;
        break; // D
  
      case 0x6A618E02:
        if(main_current_screen == MAIN_SCREEN_CHRONOMETER) thr_chronometer.enabled = true;
        break; // Play
        
      case 0xE0F44528:
        if(main_current_screen == MAIN_SCREEN_CHRONOMETER) thr_chronometer.enabled = false;
        break; // Pause
        
      case 0xC863D6C8:
        if(main_current_screen == MAIN_SCREEN_CHRONOMETER) {
          thr_chronometer.enabled = false;
          chronometer_counter = 0;
        }
        break; // Stop
    }
  }
  else
    time_adjust_cursor_blink = true;
}

void thr_buzzer_func(){
  switch(buzzer_status){
    case BZ_STATUS_OLDCLOCK:
      if(millis()/(BZ_INTERVAL * 8) % 2 == 0)
        PORTB ^= (1 << BZ_PIN);
      else
        PORTB &= ~(1 << BZ_PIN);
      break;

    default:
      PORTB &= ~(1 << BZ_PIN);
      break;
  }
}

void thr_scroll_func(){
  //if(disp_scroll_dir ? (disp_scroll_cursor_init))

  disp_clear();
  disp_setCursor(0);
  disp_print(disp_scroll_buffer.substring(disp_scroll_cursor_init, disp_scroll_cursor_init + DISP_LENGTH).c_str());

  disp_scroll_cursor_init++;

  // Disable scroll and back to the main
  if(disp_scroll_cursor_init > (disp_scroll_buffer.length() - DISP_LENGTH + 1)){
    thr_scroll.enabled = false;
    thr_main.enabled = true;
  }
}

void thr_rtc_fix_func(){
  // Read current time
  rtc_now = rtc.now();

  // Get current time
  time_adjust_year = rtc_now.year();
  time_adjust_month = rtc_now.month();
  time_adjust_day = rtc_now.day();
  time_adjust_hour = rtc_now.hour();
  time_adjust_minute = rtc_now.minute();
  time_adjust_second = rtc_now.second();

  // Back one second
  time_adjust_second -= 1;

  // Adjust the time
  rtc.adjust(DateTime(
    time_adjust_year, time_adjust_month, time_adjust_day,
    time_adjust_hour, time_adjust_minute, time_adjust_second
  ));
}

/*    *    *    *    TIMER2    *    *    *    */
ISR(TIMER2_COMPA_vect){
  // Clean display
  PORTD = 0x00;                                   // Sets all PORTD pins to LOW

  // Brightness control
  if((disp_count/DISP_LENGTH) < ((int) disp_brightness_buffer.getAverage())){
    PORTB |= 0b00111100;                          // Puts pins PB2, PB3, PB4, PB5 to HIGH
    PORTB ^= 0b00111100;                          // Toggle pins PB2, PB3, PB4, PB5 to LOW
  
    // Active the current digit of display
    PORTB ^= (1 << DISP_PIN_FIRST + disp_digit);   // Toggle the current digit pin to HIGH
  
    // Set display content
    PORTD ^= disp_content[disp_digit];            // Sets the display content and uses ^= to invert the bits
                                                  // (the display actives with LOW state)
  }

  // Increment the current digit register
  disp_digit++;
  disp_digit %= DISP_LENGTH;

  // Increment the brightness loop counter
  disp_count++;
  disp_count %= DISP_BR_MAX * DISP_LENGTH;
}

/* UPDATE * UPDATE  * UPDATE  * 1.1 * UPDATE * UPDATE  * UPDATE*/
// 13/09/2019 -> Friday | Programmer day

void DispTimer_enable(){
  TIMSK2 |= (1 << OCIE2A); // enable timer compare interrupt
}

void DispTimer_disable(){
  TIMSK2 &= ~(1 << OCIE2A); // disable timer compare interrupt
  PORTD = 0x00;             // Clean display
}


// Display print
void disp_setCursor(int col){
  disp_content_cursor = col % DISP_LENGTH;
}

void disp_clear(){
  for(int i = 0; i < DISP_LENGTH; i++)
    disp_content[i] = 0x00;
}

void disp_print(char c){
  c = toupper(c);
  disp_content[disp_content_cursor] = seven_seg_asciis[((int) c) - seven_seg_ascii_init]; //content[i];
  disp_content_cursor++;
}

void disp_print(const char* content){
  for(int i = 0; i < strlen(content) && disp_content_cursor < DISP_LENGTH; i++)
    disp_print(content[i]);
}

void disp_print(int num){
  char _num [11] = "";
  itoa(num, _num, 10);
  disp_print(_num);
}

// Display print on the end
void disp_printEnd(const char* content){
  disp_setCursor(DISP_LENGTH - strlen(content));
  disp_print(content);
}

void disp_printEnd(char c){ 
  disp_setCursor(DISP_LENGTH - 1);
  disp_print(c);
}

void disp_printEnd(int num){
  char _num [11] = "";
  itoa(num, _num, 10);
  disp_printEnd(_num);
}

// Scroll
void disp_scroll(String content, int interval){
  disp_scroll_cursor_init = 0;
  disp_scroll_buffer = content;
  //disp_scroll_dir = true;
  thr_scroll.enabled = true;
  thr_main.enabled = false;
  thr_scroll.setInterval(interval);
}

void disp_scroll(String content){
  disp_scroll(content, SCROLL_INTERVAL);
}
