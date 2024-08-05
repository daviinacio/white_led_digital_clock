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
#define LDR_MIN 0
#define LDR_MAX 650

#define DISP_BR_MIN 1
#define DISP_BR_MAX 64

#define DISP_BR_BUFFER_LENGTH 16
#define DISP_BR_BUFFER_INTERVAL 2000

#define MAIN_INTERVAL 100
#define MAIN_SCREEN_HOME 0x00
#define MAIN_SCREEN_LDR 0x01
#define MAIN_SCREEN_CHRONOMETER 0x02
#define MAIN_SCREEN_ADJUST_TIME 0x03
#define MAIN_SCREEN_ADJUST_RTC_FIX 0x04

#define MAIN_SCREEN_FIRST MAIN_SCREEN_LDR
#define MAIN_SCREEN_LAST MAIN_SCREEN_ADJUST_RTC_FIX

#define CHRONOMETER_INTERVAL 100

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
#define BZ_STATUS_OFF 0x00
#define BZ_STATUS_OLDCLOCK 0x01

#define PANEL_PIN A3
#define PANEL_INTERVAL 200

#define ADJUST_TIME_CURSOR_RANGE 5
#define ADJUST_RTC_FIX_CURSOR_RANGE 2

#define RTC_FIX_INTERVAL_EEPROM_ADDRESS 0x05
#define RTC_FIX_OPERATION_EEPROM_ADDRESS 0x01

// Library imports
#include <EEPROMex.h>
#include <Thread.h>
#include <ThreadController.h>

#include <Wire.h>
#include "RTClib.h"

#include "DHT.h"

#include <IRremote.hpp>

#include <Buffer.h>

// Import Modules
#include "display.hpp"
#include "panel.hpp"
#include "light-sensor.hpp"

// Modules
AnalogPanel panel = AnalogPanel(PANEL_PIN, PANEL_INTERVAL);
LightSensor ldr = LightSensor(LDR_PIN);

// Threads
ThreadController cpu = ThreadController();
Thread thr_main = Thread();
Thread thr_chronometer = Thread();

Thread thr_ldr = Thread();
Thread thr_rtc = Thread();
Thread thr_dht = Thread();
Thread thr_ir = Thread();
Thread thr_buzzer = Thread();
Thread thr_rtc_fix = Thread();

Buffer disp_brightness_buffer(DISP_BR_BUFFER_LENGTH, DISP_BR_MAX);

// RTC
RTC_DS1307 rtc;
DateTime rtc_now;

// DHT
DHT dht(DHT_PIN, DHT_TYPE);

Buffer dht_temp_buffer(DHT_BUFFER_LENGTH, DHT_INIT_VALUE);
Buffer dht_hum_buffer(DHT_BUFFER_LENGTH, DHT_INIT_VALUE);

// Main
unsigned short main_current_screen = MAIN_SCREEN_HOME;
bool main_change_loop = true;

// Chronometer
int chronometer_counter = 0;

// Buzzer
unsigned short buzzer_status = BZ_STATUS_OFF;

// RTC Fix
int rtc_fix_interval = 1;
int rtc_fix_operation = 1;

// Adjust
int adjust_cursor = 0;
int adjust_cursor_blink = false;

// Time Adjust
unsigned short time_adjust_year = 0;
unsigned short time_adjust_month = 0;
unsigned short time_adjust_day = 0;
unsigned short time_adjust_hour = 0;
unsigned short time_adjust_minute = 0;
unsigned short time_adjust_second = 0;

DateTime rtc_adjust;

void setup() {
  /*    *    *  THREADS  *    *    */

  // Initialize modules
  panel.onKeyDown(panel_onKeyDown);
  panel.onKeyPress(panel_onKeyPress);
  panel.onKeyUp(panel_onKeyUp);

  // ldr.onRun(Display.run());

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

  thr_rtc_fix.onRun(thr_rtc_fix_func);
  thr_rtc_fix.setInterval((uint32_t) rtc_fix_interval * 1000);

  // Add thread to thread controller
  cpu.add(&thr_main);
  cpu.add(&thr_chronometer);
  cpu.add(&thr_rtc);
  cpu.add(&thr_ldr);
  cpu.add(&thr_dht);
  cpu.add(&thr_ir);
  cpu.add(&thr_buzzer);
  cpu.add(&thr_rtc_fix);

  // Add modules to thread controller
  cpu.add(&panel);
  cpu.add(&Display);
  // cpu.add(&ldr);


  //if(debug)
    // thr_dht.enabled = false;
  
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

  Display.begin();

  // Wait for RTC begin
  while(!rtc.begin()){
    // Blink error for five seconds
    if(millis()/500 % 3 >= 1){
      Display.setCursor(0);
      Display.print("ERRO");
    } else {
      Display.clear();
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
  // thr_dht_func();

  /*    *     END OF BOOT     *    */
  if(debug){
    Display.disable();
  }
  else {
    Display.enable();
  }

  if(!debug){
    Display.clear();
    Display.setCursor(0);
    Display.print("DAVI");
    // Display.printEnd(1.2, 1);
    // Display.printScroll("Test", 1000);
  }
  // Display.printScroll("Test", 1000);

  // DEBUG
  //thr_ldr.enabled = false;
  disp_brightness_buffer.fill(DISP_BR_MAX);
}

void loop() {
  // Run the main thread controller
  cpu.run();
}

/*    *    *    *    MODULES   *    *    *    */
void panel_onKeyDown(AnalogPanelButton button){
  if(main_current_screen == MAIN_SCREEN_HOME || main_current_screen == MAIN_SCREEN_LDR) {
    if(button == BTN_VALUE_UP)
      disp_brightness_up();
    else if(button == BTN_VALUE_DOWN)
      disp_brightness_down();
  }
  else if(main_current_screen == MAIN_SCREEN_CHRONOMETER){
    if(button == BTN_VALUE_UP)
      thr_chronometer.enabled = !thr_chronometer.enabled;
    else if(button == BTN_VALUE_DOWN){
      if(thr_chronometer.enabled)
        thr_chronometer.enabled = false;
      else
        chronometer_counter = 0;
    }
  }
}

void panel_onKeyPress(AnalogPanelButton button, unsigned int milliseconds){
  int time = milliseconds / 250;

  if(main_current_screen == MAIN_SCREEN_HOME || main_current_screen == MAIN_SCREEN_LDR) {
    if((button == BTN_VALUE_UP || button == BTN_VALUE_DOWN) && time == 4)
      disp_brightness_auto();
  }
}

void panel_onKeyUp(AnalogPanelButton button, unsigned int milliseconds){
  int time = milliseconds / 250;

  if(button == BTN_HOME){
    if(time < 2) {
      main_current_screen = MAIN_SCREEN_HOME;
      Display.clearScroll();
    }
    return;
  }

  if(!(button == BTN_FUNC_LEFT || button == BTN_FUNC_RIGHT)) return;

  if(button == BTN_FUNC_LEFT){
    main_current_screen--;
    if(main_current_screen < MAIN_SCREEN_FIRST)
      main_current_screen = MAIN_SCREEN_LAST;
  }
  else if(button == BTN_FUNC_RIGHT){
    main_current_screen++;
    if(main_current_screen > MAIN_SCREEN_LAST)
      main_current_screen = MAIN_SCREEN_FIRST;
  }

  print_current_screen_name();

  if(main_current_screen == MAIN_SCREEN_ADJUST_TIME) {
    screen_adjust_time__initialize();
  }
}

void print_current_screen_name(){
  switch(main_current_screen){
    case MAIN_SCREEN_LDR: Display.printScroll("    BRILHO    ", 150); break;
    case MAIN_SCREEN_CHRONOMETER: Display.printScroll("    CRONOMETRO    ", 150); break;
    case MAIN_SCREEN_ADJUST_TIME: Display.printScroll("    SET TIME    ", 150); break;
    case MAIN_SCREEN_ADJUST_RTC_FIX: Display.printScroll("    RTC FIX    ", 150); break;
  }
}


/*    *    *      SCREEN EVENTS     *    *    */
void screen_adjust_time__initialize(){
  rtc_now = rtc.now();
  time_adjust_year = rtc_now.year();
  time_adjust_month = rtc_now.month();
  time_adjust_day = rtc_now.day();
  time_adjust_hour = rtc_now.hour();
  time_adjust_minute = rtc_now.minute();
}


/*    *    *    *    THREAD    *    *    *    */

void thr_main_func() {
  if(debug){
    // PinMode display digits pins
    PORTD = 0b00000010;                   // Set all PortD pins to HIGH
    PORTB = 0b00111100;              // Set all PortB pins to HIGH
    return;
  }

  Display.setTimeSeparator(false);
  long m = millis();
  
  switch(main_current_screen){
    case MAIN_SCREEN_HOME:
      thr_main.setInterval(MAIN_INTERVAL);
    
      // Initial interval
      if(m <= 2000){ /* Do nothing */ } else
      
      // Temperature                  // Each 20s, runs on 16s, per 2s
      if(m / 2000 % 10 == 8 && dht_temp_buffer.getAverage() != DHT_INIT_VALUE && main_change_loop){
        Display.setCursor(0);
        Display.print((int) dht_temp_buffer.getAverage());
        Display.printEnd("*C");
      } else
      
      // Humidity                     // Each 20s, runs on 18s, per 2s
      if(m / 2000 % 10 == 9 && dht_hum_buffer.getAverage() != DHT_INIT_VALUE && main_change_loop){
        Display.clear();
        Display.setCursor(0);
        Display.print("H");
        Display.printEnd(dht_hum_buffer.getAverage());
      }
      
      // Hours and Minutes
      else {                          // Runs when others 'IFs' are false
        Display.setTimeSeparator(true);
        Display.setCursor(0);
        
        if(rtc_now.hour() < 10)
          Display.print(0);
        Display.print(rtc_now.hour());

        if(rtc_now.minute() < 10)
          Display.print(0);
        Display.print(rtc_now.minute());
      }
      break;

    case MAIN_SCREEN_LDR:
      Display.setCursor(0);
      Display.print("BR");

      if(Display.getBrightness() < 10)
        Display.print(" ");
      Display.printEnd(Display.getBrightness());
      break;

    case MAIN_SCREEN_CHRONOMETER:
      thr_main.setInterval(CHRONOMETER_INTERVAL);
      Display.clear();

      if(chronometer_counter < 600){
        Display.printEnd(chronometer_counter * 0.1, 1);
      }
      else if(chronometer_counter < 36000) {
        Display.setCursor(0);
        unsigned short minute = (chronometer_counter / 10) / 60;
        unsigned short seconds = (chronometer_counter / 10) % 60;

        if(minute < 10)
          Display.print(0);
        Display.print(minute);

        if(seconds < 10)
          Display.print(0);
        Display.print(seconds);
        
        Display.setTimeSeparator((millis() / 500 % 2 == 1) || !thr_chronometer.enabled);
      }
      else {
        thr_chronometer.enabled = false;
        chronometer_counter = 0;
      }

      break;

    case MAIN_SCREEN_ADJUST_TIME:
      Display.setCursor(0);
    
      if(adjust_cursor == 0 || adjust_cursor == 1){  // Minutes & Hours
        Display.setCursor(0);

        if((adjust_cursor == 1 && millis()/500 % 3 == 0) && adjust_cursor_blink){   // Blink 1/3 on focus
          Display.print("  ");
        }
        else {
          if(time_adjust_hour < 10)
            Display.print(0);
          Display.print(time_adjust_hour);
        }

        if((adjust_cursor == 0 && millis()/500 % 3 == 0) && adjust_cursor_blink){   // Blink 1/3 on focus
          Display.print("  ");
        }
        else {
          if(time_adjust_minute < 10)
            Display.print(0);
          Display.print(time_adjust_minute);
        }
      }
      else
      if(adjust_cursor == 2){  // Day
        Display.setCursor(0);
        Display.print("D ");

        if((millis()/500 % 3 == 0) && adjust_cursor_blink){  // Blink 1/3 on focus
          Display.print("  ");
        }
        else {
          if(time_adjust_day % 100 < 10)
            Display.print(" ");
            
          Display.printEnd(time_adjust_day);
        }
      }
      else
      if(adjust_cursor == 3){  // Month
        Display.setCursor(0);
        Display.print("M ");

        if((millis()/500 % 3 == 0) && adjust_cursor_blink){  // Blink 1/3 on focus
          Display.print("  ");
        }
        else {
          if(time_adjust_month % 100 < 10)
            Display.print(" ");
            
          Display.printEnd(time_adjust_month);
        }
      }
      else
      if(adjust_cursor == 4){  // Year
        Display.setCursor(0);
        Display.print("Y ");

        if((millis()/500 % 3 == 0) && adjust_cursor_blink){  // Blink 1/3 on focus
          Display.print("  ");
        }
        else {
          if(time_adjust_year % 100 < 10)
            Display.print(" ");
            
          Display.printEnd((int) time_adjust_year % 100);
        }
      }
      break;

    case MAIN_SCREEN_ADJUST_RTC_FIX:
      if(adjust_cursor == 0){  // Interval
        Display.setCursor(0);
        
        if((millis()/500 % 3 == 0) && adjust_cursor_blink){  // Blink 1/3 on focus
          Display.clear();
        }
        else {
          Display.printEnd(rtc_fix_interval);
        }
      }
      else
      if(adjust_cursor == 1){ // Opetarion
        Display.setCursor(0);
        Display.print("OP ");
        
        if((millis()/500 % 3 == 0) && adjust_cursor_blink){  // Blink 1/3 on focus
          Display.print(" ");
        }
        else {
          Display.printEnd(rtc_fix_operation);
        }
      }
      break;

    default:
      Display.clear();
      Display.setCursor(0);
      Display.print("MERR");
      break;
  }
}

void thr_chronometer_func(){
  chronometer_counter++;
  chronometer_counter %= (int) pow(10, DISP_LENGTH);
}

void thr_ldr_func(){
  // Make the ldr sensor read
  int br_read = map(analogRead(LDR_PIN), LDR_MIN, LDR_MAX, DISP_BR_MIN, DISP_BR_MAX);

  // Avoid out of range problems
  if(br_read > DISP_BR_MAX) br_read = DISP_BR_MAX; else
  if(br_read < DISP_BR_MIN) br_read = DISP_BR_MIN;

  disp_brightness_buffer.insert(br_read);
  Display.setBrightness(disp_brightness_buffer.getAverage());
}

void thr_rtc_func(){
  rtc_now = rtc.now();
}

void thr_dht_func(){
  // Disable display timer while dht read
  if(!debug) Display.disable();
  float dht_temp_read = dht.readTemperature();
  float dht_hum_read = dht.readHumidity();
  if(!debug) Display.enable();

  // Ignore read errors
  if(isnan(dht_temp_read) || isnan(dht_hum_read)) return;

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
      // case 0x0:  // Exit
        if(main_current_screen == MAIN_SCREEN_ADJUST_TIME){
          // rtc_now

          rtc.adjust(DateTime(
            time_adjust_year, time_adjust_month, time_adjust_day,
            time_adjust_hour, time_adjust_minute, 1
          ));
          Display.printScroll("SAVE", 1500);
        }
        else
        if(main_current_screen == MAIN_SCREEN_ADJUST_RTC_FIX){
          EEPROM.writeInt(RTC_FIX_INTERVAL_EEPROM_ADDRESS, rtc_fix_interval);
          EEPROM.writeInt(RTC_FIX_OPERATION_EEPROM_ADDRESS, rtc_fix_operation);

          thr_rtc_fix.setInterval((uint32_t) rtc_fix_interval * 1000);
          
          Display.printScroll("SAVE", 1500);
        }

        main_current_screen = MAIN_SCREEN_HOME;
        thr_rtc.enabled = true;
        thr_rtc_fix.enabled = true;
        break;
      
      case 0xF8070707:  //disp_brightness += DISP_BR_MAX/8; thr_ldr.enabled = false; 
        disp_brightness_up();
        break; // Vol+
        
      case 0xF40B0707:  //disp_brightness -= DISP_BR_MAX/8; thr_ldr.enabled = false;
        disp_brightness_down();
        break; // Vol-
        
      case 0xF00F0707:
        disp_brightness_auto();
        break;
        // Mute

      // case 0xFE010707:  Display.printScroll("   DESENVOLVIDO POR DAVI INACIO    ", 300); break; // Content
      // case 0xC13E0707:  Display.printScroll("    - FELIZ DIA DO PROGRAMADOR -    ", 250); break; // Content
  
      case 0xFB040707:
        main_current_screen = MAIN_SCREEN_LDR;
        print_current_screen_name();
      break; // 1
      
      case 0xFA050707:
        main_current_screen = MAIN_SCREEN_CHRONOMETER;
        print_current_screen_name();
        break; // 2
      case 0xF9060707:
        if(main_current_screen != MAIN_SCREEN_ADJUST_RTC_FIX){
          thr_rtc.enabled = false;
          thr_rtc_fix.enabled = false;
          adjust_cursor_blink = true;
          main_current_screen = MAIN_SCREEN_ADJUST_RTC_FIX;
          adjust_cursor = 0;

          print_current_screen_name();
        }
        break; // 3
      case 0xF7080707:
        if (rtc.isrunning()) {
          screen_adjust_time__initialize();
        }
        
        thr_rtc.enabled = false;
        thr_rtc_fix.enabled = false;
        adjust_cursor_blink = true;
        main_current_screen = MAIN_SCREEN_ADJUST_TIME;
        adjust_cursor = 1;

        print_current_screen_name();
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


void debug_print_button_name(int button){
  switch(button){
    case BTN_HOME: Serial.print("'home'"); break;
    case BTN_VALUE_UP: Serial.print("'value up'"); break;
    case BTN_VALUE_DOWN: Serial.print("'value down'"); break;
    case BTN_FUNC_LEFT: Serial.print("'func left'"); break;
    case BTN_FUNC_RIGHT: Serial.print("'func right'"); break;
  }
}

void disp_brightness_up(){
  Display.incrementBrightness();
  thr_ldr.enabled = false;
  disp_brightness_buffer.fill(Display.getBrightness());
}

void disp_brightness_down(){
  Display.decrementBrightness();
  thr_ldr.enabled = false;
  disp_brightness_buffer.fill(Display.getBrightness());
}

void disp_brightness_auto(){
  if(!thr_ldr.enabled)
    Display.printScroll("AUTO", 1500);
  thr_ldr.enabled = true; 
}
