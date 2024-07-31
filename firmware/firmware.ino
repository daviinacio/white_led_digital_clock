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

bool debug = false;

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
#define MAIN_SCREEN_ADJUST_TIME 1003
#define MAIN_SCREEN_ADJUST_RTC_FIX 1004

#define CHRONOMETER_INTERVAL 1000

#define RTC_INTERVAL 1000

#define DHT_INIT_VALUE -255
#define DHT_PIN A0
#define DHT_TYPE DHT11
#define DHT_BUFFER_LENGTH 4
#define DHT_BUFFER_INTERVAL 60000

#define IR_PIN A1
#define IR_INTERVAL 200

#define BZ_PIN PB1
#define BZ_INTERVAL 62
#define BZ_STATUS_OFF 1100
#define BZ_STATUS_OLDCLOCK 1101

#define PANEL_PIN A3
#define PANEL_INTERVAL 50
#define PANEL_DEFAULT 0
#define PANEL_BTN_HOME 1
#define PANEL_RANGE_BTN_HOME (value < 50)
#define PANEL_BTN_VALUE_UP 2
#define PANEL_RANGE_BTN_VALUE_UP (value >= 750 && value < 900)
#define PANEL_BTN_VALUE_DOWN 3
#define PANEL_RANGE_BTN_VALUE_DOWN (value >= 550 && value < 750)
#define PANEL_BTN_FUNC_LEFT 4
#define PANEL_RANGE_BTN_FUNC_LEFT (value >= 350 && value < 550)
#define PANEL_BTN_FUNC_RIGHT 5
#define PANEL_RANGE_BTN_FUNC_RIGHT (value >= 100 && value < 300)

#define SCROLL_INTERVAL 350

#define ADJUST_TIME_CURSOR_RANGE 5
#define ADJUST_RTC_FIX_CURSOR_RANGE 2

#define RTC_FIX_INTERVAL_EEPROM_ADDRESS 0x05
#define RTC_FIX_OPERATION_EEPROM_ADDRESS 0x01

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
  0b01111100,   // U
  0b00111000,   // V
  0b01010100,   // W
  0b10010010,   // X
  0b01110110,   // Y
  0b11011010    // Z
};

// Library imports
#include <EEPROMex.h>
#include <Thread.h>
#include <ThreadController.h>

#include <Wire.h>
#include "RTClib.h"

#include "DHT.h"

//#include <IRremote.h>
#include <IRremote.hpp>

#include <Buffer.h>

// #include "display.hpp"

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
Thread thr_panel = Thread();

// Display
byte disp_content [DISP_LENGTH];
int disp_decimal_position = 0;
bool disp_time_marker = false;

Buffer disp_brightness_buffer(DISP_BR_BUFFER_LENGTH, DISP_BR_MAX);

int disp_count = 0; 
int disp_digit = 0;

int disp_content_cursor = 0;

// RTC
RTC_DS1307 rtc;
DateTime rtc_now;

// DHT
DHT dht(DHT_PIN, DHT_TYPE);

Buffer dht_temp_buffer(DHT_BUFFER_LENGTH, DHT_INIT_VALUE);
Buffer dht_hum_buffer(DHT_BUFFER_LENGTH, DHT_INIT_VALUE);

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

// RTC Fix
int rtc_fix_interval = 1;
int rtc_fix_operation = 1;

// Panel
long panel_milliseconds_pressing = 0;
int panel_btn_pressing = PANEL_DEFAULT;

// Adjust
int adjust_cursor = 0;
int adjust_cursor_blink = false;

// Time Adjust
int time_adjust_year = 0;
int time_adjust_month = 0;
int time_adjust_day = 0;
int time_adjust_hour = 0;
int time_adjust_minute = 0;
int time_adjust_second = 0;

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
  
  if(!debug){
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
  }

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
  thr_rtc_fix.setInterval((uint32_t) rtc_fix_interval * 1000);

  thr_panel.onRun(thr_panel_func);
  thr_panel.setInterval(PANEL_INTERVAL);


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
  cpu.add(&thr_panel);

  //if(debug)
    thr_dht.enabled = false;
  
  
  /*    *  LIBRARY BEGINNERS  *    */

  Wire.begin();
  dht.begin();
  IrReceiver.begin(IR_PIN);
  //ir_recv.enableIRIn();
  if(debug){
    Serial.begin(9600);
    delay(50);
    Serial.println("Debug mode initialized");
  }

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
    main_current_screen = MAIN_SCREEN_ADJUST_TIME;
    adjust_cursor = 1;
    thr_rtc.enabled = false;
    thr_rtc_fix.enabled = false;
  }

  /*    *     EEPROM READ     *    */
  
  if(EEPROM.isReady()){
    rtc_fix_operation = EEPROM.readInt(
      RTC_FIX_OPERATION_EEPROM_ADDRESS
    );
    
    rtc_fix_interval = EEPROM.readInt(
      RTC_FIX_INTERVAL_EEPROM_ADDRESS
    );

    if(rtc_fix_interval == 0 && main_current_screen == MAIN_SCREEN_HOME) {
      // Goto rtc fix adjust screen
      main_current_screen = MAIN_SCREEN_ADJUST_RTC_FIX;
      adjust_cursor = 0;
      thr_rtc.enabled = false;
      thr_rtc_fix.enabled = false;
    }
    else
    if(rtc_fix_interval >= 1){
      thr_rtc_fix.setInterval((uint32_t) rtc_fix_interval * 1000);
    }
  }

  /*    *   THREAD FIRST RUN  *    */
  
  thr_ldr_func();
  thr_rtc_func();
  thr_dht_func();

  /*    *     END OF BOOT     *    */

  thr_scroll.enabled = false;

  if(!debug){
    disp_clear();
    disp_setCursor(0);
    disp_print((char*)"DAVI");
  }

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
  if(debug){
    // PinMode display digits pins
    PORTD = 0b00000010;                   // Set all PortD pins to HIGH
    PORTB = 0b00111100;              // Set all PortB pins to HIGH
    return;
  }

  disp_time_marker = false;
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
        disp_time_marker = true;
        //disp_time_marker = m / 1000 % 2 == 0;
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

    case MAIN_SCREEN_ADJUST_TIME:
      disp_setCursor(0);
    
      if(adjust_cursor == 0 || adjust_cursor == 1){  // Minutes & Hours
        disp_setCursor(0);

        if((adjust_cursor == 1 && millis()/500 % 3 == 0) && adjust_cursor_blink){   // Blink 1/3 on focus
          disp_print((char*)"  ");
        }
        else {
          if(time_adjust_hour < 10)
            disp_print(0);
          disp_print(time_adjust_hour);
        }

        if((adjust_cursor == 0 && millis()/500 % 3 == 0) && adjust_cursor_blink){   // Blink 1/3 on focus
          disp_print((char*)"  ");
        }
        else {
          if(time_adjust_minute < 10)
            disp_print(0);
          disp_print(time_adjust_minute);
        }
      }
      else
      if(adjust_cursor == 2){  // Day
        disp_setCursor(0);
        disp_print((char*)"D ");

        if((millis()/500 % 3 == 0) && adjust_cursor_blink){  // Blink 1/3 on focus
          disp_print((char*)"  ");
        }
        else {
          if(time_adjust_day % 100 < 10)
            disp_print((char*)" ");
            
          disp_printEnd(time_adjust_day);
        }
      }
      else
      if(adjust_cursor == 3){  // Month
        disp_setCursor(0);
        disp_print((char*)"M ");

        if((millis()/500 % 3 == 0) && adjust_cursor_blink){  // Blink 1/3 on focus
          disp_print((char*)"  ");
        }
        else {
          if(time_adjust_month % 100 < 10)
            disp_print((char*)" ");
            
          disp_printEnd(time_adjust_month);
        }
      }
      else
      if(adjust_cursor == 4){  // Year
        disp_setCursor(0);
        disp_print((char*)"Y ");

        if((millis()/500 % 3 == 0) && adjust_cursor_blink){  // Blink 1/3 on focus
          disp_print((char*)"  ");
        }
        else {
          if(time_adjust_year % 100 < 10)
            disp_print((char*)" ");
            
          disp_printEnd(time_adjust_year % 100);
        }
      }
      break;

    case MAIN_SCREEN_ADJUST_RTC_FIX:
      if(adjust_cursor == 0){  // Interval
        disp_setCursor(0);
        
        if((millis()/500 % 3 == 0) && adjust_cursor_blink){  // Blink 1/3 on focus
          disp_clear();
        }
        else {
          disp_printEnd(rtc_fix_interval);
        }
      }
      else
      if(adjust_cursor == 1){ // Opetarion
        disp_setCursor(0);
        disp_print((char*)"OP ");
        
        if((millis()/500 % 3 == 0) && adjust_cursor_blink){  // Blink 1/3 on focus
          disp_print((char*)" ");
        }
        else {
          disp_printEnd(rtc_fix_operation);
        }
      }
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
  float dht_temp_read = dht.readTemperature();
  float dht_hum_read = dht.readHumidity();
  DispTimer_enable();

  // Ignore read errors
  //if(dht.getStatus() != DHT::ERROR_NONE) return;

  // Avoid first read problems
  if(dht_temp_buffer.empty()) dht_temp_buffer.fill((int) dht_temp_read);
  else                        dht_temp_buffer.insert((int) dht_temp_read);

  if(dht_hum_buffer.empty())  dht_hum_buffer.fill((int) dht_hum_read);
  else                        dht_hum_buffer.insert((int) dht_hum_read);

  if(debug){
    Serial.print("DHT sensor: T ");
    Serial.print((int) dht_temp_read);
    Serial.print(", H ");
    Serial.println((int) dht_hum_read);
  }
}

void thr_ir_func(){
  if(IrReceiver.decode()){
    // Read the ir HEX value
    unsigned long value = IrReceiver.decodedIRData.decodedRawData;
    // Receive the next value
    IrReceiver.resume();

    if(debug){
      Serial.print("IR sensor received: 0x");
      Serial.println(value, HEX);
    }

    // Temporary variable
    int tp;
    
    adjust_cursor_blink = false;

    switch(value){
      case 0x9F600707: // Up
        if(main_current_screen == MAIN_SCREEN_HOME)
          buzzer_status = BZ_STATUS_OLDCLOCK;
        else
        if(main_current_screen == MAIN_SCREEN_ADJUST_TIME){
          switch(adjust_cursor){
            case 0: if(time_adjust_minute < 59) time_adjust_minute++; break;
            case 1: if(time_adjust_hour < 23) time_adjust_hour++; break;
            case 2: if(time_adjust_day < 31) time_adjust_day++; break;
            case 3: if(time_adjust_month < 12) time_adjust_month++; break;
            case 4: if(time_adjust_year < 2100) time_adjust_year++; break;
          }
        }
        else
        if(main_current_screen == MAIN_SCREEN_ADJUST_RTC_FIX){
          switch(adjust_cursor){
            case 0: if(rtc_fix_interval < 9999) rtc_fix_interval++; break;
            case 1: if(rtc_fix_operation < 2) rtc_fix_operation++; break;
          }
        }
        break;

      case 0x9E610707: // Down
        if(main_current_screen == MAIN_SCREEN_HOME)
          dht_temp_buffer.fill(-42);
        else
        if(main_current_screen == MAIN_SCREEN_ADJUST_TIME){
          switch(adjust_cursor){
            case 0: if(time_adjust_minute > 0) time_adjust_minute--; break;
            case 1: if(time_adjust_hour > 0) time_adjust_hour--; break;
            case 2: if(time_adjust_day > 0) time_adjust_day--; break;
            case 3: if(time_adjust_month > 0) time_adjust_month--; break;
            case 4: if(time_adjust_year > 2000) time_adjust_year--; break;
          }
        }
        else
        if(main_current_screen == MAIN_SCREEN_ADJUST_RTC_FIX){
          switch(adjust_cursor){
            case 0: if(rtc_fix_interval > 1) rtc_fix_interval--; break;
            case 1: if(rtc_fix_operation > 0) rtc_fix_operation--; break;
          }
        }
        break; 

      case 0x9A650707: // Left
        if(main_current_screen == MAIN_SCREEN_ADJUST_TIME){
          if(adjust_cursor < ADJUST_TIME_CURSOR_RANGE - 1)
            adjust_cursor++;
        }
        else
        if(main_current_screen == MAIN_SCREEN_ADJUST_RTC_FIX){
          if(adjust_cursor < ADJUST_RTC_FIX_CURSOR_RANGE - 1)
            adjust_cursor++;
        }
        break;

      case 0x9D620707: // Right
        if(main_current_screen == MAIN_SCREEN_ADJUST_TIME){
          if(adjust_cursor > 0)
            adjust_cursor--;
        }
        else
        if(main_current_screen == MAIN_SCREEN_ADJUST_RTC_FIX){
          if(adjust_cursor > 0)
            adjust_cursor--;
        }
        break;

      case 0x97680707:  // Center
        if(main_current_screen == MAIN_SCREEN_HOME)
          buzzer_status = BZ_STATUS_OFF;
        else if(main_current_screen == MAIN_SCREEN_CHRONOMETER)
          thr_chronometer.enabled = !thr_chronometer.enabled;
          
        break;
      
      case 0xE51A0707: // Return
      case 0x0:  // Exit
        if(main_current_screen == MAIN_SCREEN_ADJUST_TIME){
          rtc.adjust(DateTime(
            time_adjust_year, time_adjust_month, time_adjust_day,
            time_adjust_hour, time_adjust_minute, 1
          ));
          disp_scroll("SAVE", 1500);
        }
        else
        if(main_current_screen == MAIN_SCREEN_ADJUST_RTC_FIX){
          EEPROM.writeInt(RTC_FIX_INTERVAL_EEPROM_ADDRESS, rtc_fix_interval);
          EEPROM.writeInt(RTC_FIX_OPERATION_EEPROM_ADDRESS, rtc_fix_operation);

          thr_rtc_fix.setInterval((uint32_t) rtc_fix_interval * 1000);
          
          disp_scroll("SAVE", 1500);
        }

        main_current_screen = MAIN_SCREEN_HOME;
        thr_rtc.enabled = true;
        thr_rtc_fix.enabled = true;
        break;
      
      case 0xF8070707:  //disp_brightness += DISP_BR_MAX/8; thr_ldr.enabled = false; 
        thr_ldr.enabled = false;
        
        tp = disp_brightness_buffer.getAverage();
        tp += DISP_BR_MAX/8;

        if(tp % 2 != 0) tp -= 1;
        if(tp > DISP_BR_MAX) tp = DISP_BR_MAX;

        disp_brightness_buffer.fill(tp);
        break; // Vol+
        
      case 0xF40B0707:  //disp_brightness -= DISP_BR_MAX/8; thr_ldr.enabled = false;
        thr_ldr.enabled = false;
        
        tp = disp_brightness_buffer.getAverage();
        tp -= DISP_BR_MAX/8;
        if(tp < DISP_BR_MIN) tp = DISP_BR_MIN;

        disp_brightness_buffer.fill(tp);
        break; // Vol-
        
      case 0xF00F0707:
        if(!thr_ldr.enabled)
          disp_scroll("AUTO", 1500);
        thr_ldr.enabled = true; break;
        // Mute

      case 0xFE010707:  disp_scroll("   DESENVOLVIDO POR DAVI INACIO    ", 300); break; // Content
      case 0xC13E0707:  disp_scroll("    - FELIZ DIA DO PROGRAMADOR -    ", 250); break; // Content
  
      case 0xFB040707:
        if(main_current_screen != MAIN_SCREEN_LDR)
          disp_scroll("   BRILHO   ");
          
        main_current_screen = MAIN_SCREEN_LDR;
      break; // 1
      
      case 0xFA050707:
        if(main_current_screen != MAIN_SCREEN_CHRONOMETER)
          disp_scroll("   CRONOMETRO   ");
        main_current_screen = MAIN_SCREEN_CHRONOMETER;
        break; // 2
      case 0xF9060707:
        if(main_current_screen != MAIN_SCREEN_ADJUST_RTC_FIX){
          disp_scroll("   RTC FIX   ");
          
          thr_rtc.enabled = false;
          thr_rtc_fix.enabled = false;
          adjust_cursor_blink = true;
          main_current_screen = MAIN_SCREEN_ADJUST_RTC_FIX;
          adjust_cursor = 0;
        }
        break; // 3
      case 0xF7080707:
        if(main_current_screen != MAIN_SCREEN_ADJUST_TIME)
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
        adjust_cursor_blink = true;
        main_current_screen = MAIN_SCREEN_ADJUST_TIME;
        adjust_cursor = 1;
        break; // 4
  
      case 0xEC130707:
        if(main_current_screen == MAIN_SCREEN_CHRONOMETER) {
          thr_chronometer.enabled = false;
          chronometer_counter = 0;
        }
        break; // Stop
    }
  }
  else
    adjust_cursor_blink = true;
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
  if(rtc_fix_operation == 0){
    thr_rtc_fix.enabled = false;
    return;
  }
  
  // Read current time
  rtc_now = rtc.now();

  // Get current time
  time_adjust_year = rtc_now.year();
  time_adjust_month = rtc_now.month();
  time_adjust_day = rtc_now.day();
  time_adjust_hour = rtc_now.hour();
  time_adjust_minute = rtc_now.minute();
  time_adjust_second = rtc_now.second();

  // Fix one second
  time_adjust_second += (
    rtc_fix_operation == 2 ?
      2 : -1
  );

  // Adjust the time
  rtc.adjust(DateTime(
    time_adjust_year, time_adjust_month, time_adjust_day,
    time_adjust_hour, time_adjust_minute, time_adjust_second
  ));
}

void thr_panel_func(){
  // Detect Button
  int button = PANEL_DEFAULT;
  int value = analogRead(PANEL_PIN);

  if(PANEL_RANGE_BTN_HOME) button = PANEL_BTN_HOME;
  else if(PANEL_RANGE_BTN_VALUE_UP) button = PANEL_BTN_VALUE_UP;
  else if(PANEL_RANGE_BTN_VALUE_DOWN) button = PANEL_BTN_VALUE_DOWN;
  else if(PANEL_RANGE_BTN_FUNC_LEFT) button = PANEL_BTN_FUNC_LEFT;
  else if(PANEL_RANGE_BTN_FUNC_RIGHT) button = PANEL_BTN_FUNC_RIGHT;

  if(panel_btn_pressing == button && button != PANEL_DEFAULT){
    panel_milliseconds_pressing += PANEL_INTERVAL;
  }
  else {
    panel_btn_pressing = button;
    panel_milliseconds_pressing = 0;
  }

  if(button == PANEL_DEFAULT) return;

  if(debug){
    Serial.print("Analog: ");
    Serial.print(value);
    Serial.print(" | Button: ");
    switch(button){
      case PANEL_BTN_HOME: Serial.print("'home'"); break;
      case PANEL_BTN_VALUE_UP: Serial.print("'value up'"); break;
      case PANEL_BTN_VALUE_DOWN: Serial.print("'value down'"); break;
      case PANEL_BTN_FUNC_LEFT: Serial.print("'func left'"); break;
      case PANEL_BTN_FUNC_RIGHT: Serial.print("'func right'"); break;
    }
    Serial.print(" | Time pressing: ");
    Serial.print(panel_milliseconds_pressing / 1000);
    Serial.println("s");
  }
}

/*    *    *    *    TIMER2    *    *    *    */
ISR(TIMER2_COMPA_vect){
  if(debug)
    return;

  // Clean display
  PORTD = 0x00;                                   // Sets all PORTD pins to LOW

  // Brightness control
  if((disp_count/DISP_LENGTH) < ((int) disp_brightness_buffer.getAverage())){
    PORTB |= 0b00111100;                          // Puts pins PB2, PB3, PB4, PB5 to HIGH
    PORTB ^= 0b00111100;                          // Toggle pins PB2, PB3, PB4, PB5 to LOW
  
    // Active the current digit of display
    PORTB ^= (1 << DISP_PIN_FIRST + disp_digit);  // Toggle the current digit pin to HIGH
  
    // Set display content
    PORTD ^= disp_content[disp_digit];            // Sets the display content and uses ^= to invert the bits
                                                  // (the display actives with LOW state)

    // Set time marker
    if(disp_time_marker && disp_digit == DISP_LENGTH -1)
      PORTD ^= (1 << 0);

    // Set decimal position
    if(disp_decimal_position == disp_digit + 1 && disp_decimal_position <= 3) {
      PORTD ^= (1 << 0);
    }
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
  if(debug) return;
  TIMSK2 |= (1 << OCIE2A); // enable timer compare interrupt
}

void DispTimer_disable(){
  if(debug) return;
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
  disp_time_marker = false;
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
