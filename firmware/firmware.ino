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

// Disables multiplex display, and enables Serial print
bool debug_mode = false;

#define LDR_PIN A2
#define LDR_MIN 0
#define LDR_MAX 650

#define DISP_BR_MIN 1
#define DISP_BR_MAX 64

#define DISP_BR_BUFFER_LENGTH 16
#define DISP_BR_BUFFER_INTERVAL 2000
#define DISP_BRINK_INTERVAL 300

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
#define PANEL_INTERVAL 100

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
// Import Modules
#include "display.hpp"
#include "panel.hpp"
#include "infrared.hpp"
#include "utils.hpp"
#include "buffer.hpp"

// #include "light-sensor.hpp"

// Modules
AnalogPanel panel = AnalogPanel(PANEL_PIN, PANEL_INTERVAL);
InfraredSensor ir = InfraredSensor(IR_PIN);
// LightSensor ldr = LightSensor(LDR_PIN);


// Threads
ThreadController cpu = ThreadController();
Thread thr_main = Thread();
Thread thr_chronometer = Thread();

Thread thr_ldr = Thread();
Thread thr_rtc = Thread();
Thread thr_dht = Thread();
// Thread thr_ir = Thread();
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
unsigned short main_cursor = 0;
bool main_cursor_blink = false;

// Chronometer
unsigned int chronometer_counter = 0;

// Buzzer
unsigned short buzzer_status = BZ_STATUS_OFF;

// RTC Fix
unsigned short rtc_fix_interval = 1;
unsigned short rtc_fix_operation = 1;

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

  // thr_ir.onRun(thr_ir_func);
  // thr_ir.setInterval(IR_INTERVAL);

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
  // cpu.add(&thr_ir);
  cpu.add(&ir);
  cpu.add(&thr_buzzer);
  cpu.add(&thr_rtc_fix);

  // Add modules to thread controller
  cpu.add(&panel);
  cpu.add(&Display);
  // cpu.add(&ldr);
  
  /*    *  LIBRARY BEGINNERS  *    */
  Wire.begin();
  dht.begin();
  ir.begin();
  // IrReceiver.begin(IR_PIN);
  //ir_recv.enableIRIn();
  
  if(debug_mode){
    Serial.begin(9600);
    delay(50);
    Serial.println("debug_mode mode initialized");
  }

  Display.begin();

  // Wait for RTC begin
  while(!rtc.begin()){
    // Blink error for five seconds
    if(millis()/500 % 3 >= 1){
      Display.setCursor(0);
      Display.print(F("ERRO"));
    } else {
      Display.clear();
    }
  }

  /*    *    MODULE EVENTS    *    */

  panel.onKeyDown(onKeyDown);
  panel.onKeyPress(onKeyPress);
  panel.onKeyUp(onKeyUp);
  
  ir.onKeyDown(onKeyDown);
  ir.onKeyPress(onKeyPress);
  ir.onKeyUp(onKeyUp);

  /*    *   CHECK COMPONENTS  *    */

  if (!rtc.isrunning()) {
    // Reset time
    rtc.adjust(DateTime(__DATE__, __TIME__));

    // Goto time adjust screen
    main_current_screen = MAIN_SCREEN_ADJUST_TIME;
    main_cursor = 1;
    thr_rtc.enabled = false;
    thr_rtc_fix.enabled = false;
  }

  /*    *     EEPROM READ     *    */
  
  if(EEPROM.isReady()){
    rtc_fix__read();

    if(rtc_fix_interval == 0 && main_current_screen == MAIN_SCREEN_HOME) {
      // Goto rtc fix adjust screen
      main_current_screen = MAIN_SCREEN_ADJUST_RTC_FIX;
      main_cursor = 0;
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

  if(debug_mode){
    Display.disable();
  }
  else {
    Display.enable();
  }

  if(!debug_mode){
    Display.clear();
    Display.setCursor(0);
    Display.print(F("DAVI"));
  }

  disp_brightness_buffer.fill(DISP_BR_MAX);
}

void loop() {
  // Run the main thread controller
  cpu.run();
}

/*    *    *    *    MODULES   *    *    *    */
void onKeyDown(InputKey key){
  if(main_current_screen == MAIN_SCREEN_HOME || main_current_screen == MAIN_SCREEN_LDR) {
    if(key == KEY_VALUE_UP)
      disp_brightness_up();
    else if(key == KEY_VALUE_DOWN)
      disp_brightness_down();

    if(main_current_screen != MAIN_SCREEN_HOME)
      return;

    if(key == KEY_FUNC_RIGHT)
      increment(main_cursor, 0, 2, false);
    else if(key == KEY_FUNC_LEFT)
      decrement(main_cursor, 0, 2, false);
  }
  else if(main_current_screen == MAIN_SCREEN_CHRONOMETER){
    if(key == KEY_VALUE_UP)
      thr_chronometer.enabled = !thr_chronometer.enabled;
    else if(key == KEY_VALUE_DOWN){
      if(thr_chronometer.enabled)
        thr_chronometer.enabled = false;
      else
        chronometer_counter = 0;
    }
  }
  else if(main_current_screen == MAIN_SCREEN_ADJUST_TIME){
    if(key == KEY_FUNC_LEFT)
      increment(main_cursor, 0, 4, false);
    else if(key == KEY_FUNC_RIGHT)
      decrement(main_cursor, 0, 4, false);

    screen_adjust_time__handler(key);
  }
  else if(main_current_screen == MAIN_SCREEN_ADJUST_RTC_FIX){
    if(key == KEY_FUNC_LEFT)
      increment(main_cursor, 0, 1, false);
    else if(key == KEY_FUNC_RIGHT)
      decrement(main_cursor, 0, 1, false);

    screen_adjust_rtc_fix__handler(key, 1);
  }
}

void onKeyPress(InputKey key, unsigned int milliseconds){
  int time = milliseconds / 250;

  if(main_current_screen == MAIN_SCREEN_HOME || main_current_screen == MAIN_SCREEN_LDR) {
    if((key == KEY_VALUE_UP || key == KEY_VALUE_DOWN) && time == 4)
      disp_brightness_auto();
  }
  else if(main_current_screen == MAIN_SCREEN_ADJUST_TIME && time > 1)
    screen_adjust_time__handler(key);
  else if(main_current_screen == MAIN_SCREEN_ADJUST_RTC_FIX && time > 1)
    screen_adjust_rtc_fix__handler(key, time / 2);
}

void onKeyUp(InputKey key, unsigned int milliseconds){
  int time = milliseconds / 250;

  if(Display.isScrolling() || (main_current_screen == MAIN_SCREEN_HOME && main_cursor == 0)) {
    if(key == KEY_HOME){
      increment(main_current_screen, MAIN_SCREEN_FIRST, MAIN_SCREEN_LAST, true);
      print_current_screen_name();

      if(main_current_screen != MAIN_SCREEN_HOME)
        main_cursor = 0;

      if(main_current_screen == MAIN_SCREEN_ADJUST_TIME) 
        screen_adjust_time__initialize();
      else if(main_current_screen == MAIN_SCREEN_ADJUST_RTC_FIX)
        screen_adjust_rtc_fix__initialize();
      
    }
    return;
  }
  else if(key == KEY_HOME){
    if(time < 2) {
      if(main_current_screen == MAIN_SCREEN_ADJUST_TIME)
        screen_adjust_time__submit();
      else if(main_current_screen == MAIN_SCREEN_ADJUST_RTC_FIX)
        screen_adjust_rtc_fix__submit();
      else {
        Display.clearScroll();
      }
    }
    else {
      Display.clearScroll();
      Display.printScroll(F("----"), 1500);
    }

    if(main_current_screen == MAIN_SCREEN_ADJUST_TIME)
      screen_adjust_time__destroy();
    else if(main_current_screen == MAIN_SCREEN_ADJUST_RTC_FIX)
      screen_adjust_rtc_fix__destroy();
    

    main_current_screen = MAIN_SCREEN_HOME;
    main_cursor = 0;


    return;
  }

  if(main_current_screen == MAIN_SCREEN_ADJUST_TIME){
    // main_cursor_blink = true;
  }
}

void print_current_screen_name(){
  switch(main_current_screen){
    case MAIN_SCREEN_LDR: Display.printScroll(F("    BRILHO    "), 150); break;
    case MAIN_SCREEN_CHRONOMETER: Display.printScroll(F("    CRONOMETRO    "), 150); break;
    case MAIN_SCREEN_ADJUST_TIME: Display.printScroll(F("    SET TIME    "), 150); break;
    case MAIN_SCREEN_ADJUST_RTC_FIX: Display.printScroll(F("    RTC FIX    "), 150); break;
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
  // main_cursor_blink = true;

  thr_rtc_fix.enabled = false;
  thr_rtc.enabled = false;
}

void screen_adjust_time__destroy(){
  thr_rtc_fix.enabled = true;
  thr_rtc.enabled = true;
}

void screen_adjust_time__handler(InputKey key){
  if(main_cursor == 0) {        // Minute
    if(key == KEY_VALUE_UP)
      increment(time_adjust_minute, 0, 59, true);
    else if(key == KEY_VALUE_DOWN)
      decrement(time_adjust_minute, 0, 59, true);
  }
  else if(main_cursor == 1) {   // Hour
    if(key == KEY_VALUE_UP)
      increment(time_adjust_hour, 0, 23, true);
    else if(key == KEY_VALUE_DOWN)
      decrement(time_adjust_hour, 0, 23, true);
  }
  else if(main_cursor == 2) {   // Day
    if(key == KEY_VALUE_UP)
      increment(time_adjust_day, 1, month_last_day(time_adjust_year, time_adjust_month), true);
    else if(key == KEY_VALUE_DOWN)
      decrement(time_adjust_day, 1, month_last_day(time_adjust_year, time_adjust_month), true);
  }
  else if(main_cursor == 3) {   // Month
    if(key == KEY_VALUE_UP)
      increment(time_adjust_month, 1, 12, true);
    else if(key == KEY_VALUE_DOWN)
      decrement(time_adjust_month, 1, 12, true);

    range(time_adjust_day, 1, month_last_day(time_adjust_year, time_adjust_month));
  }
  else if(main_cursor == 4) {   // Year
    if(key == KEY_VALUE_UP)
      increment(time_adjust_year, 2000, 2038, true);
    else if(key == KEY_VALUE_DOWN)
      decrement(time_adjust_year, 2000, 2038, true);

    range(time_adjust_day, 1, month_last_day(time_adjust_year, time_adjust_month));
  }
}

void screen_adjust_time__submit(){
  rtc.adjust(DateTime(
    time_adjust_year, time_adjust_month, time_adjust_day,
    time_adjust_hour, time_adjust_minute, 0
  ));
  Display.printScroll(F("SAVE"), 1500);
}



void screen_adjust_rtc_fix__initialize(){
  rtc_fix__read();
  thr_rtc_fix.enabled = false;
  thr_rtc.enabled = false;
}

void screen_adjust_rtc_fix__destroy(){
  rtc_fix__read();
  thr_rtc_fix.enabled = true;
  thr_rtc.enabled = true;
}

void screen_adjust_rtc_fix__handler(InputKey key, unsigned short mult){
  if(main_cursor == 0) {        // Interval
    if(key == KEY_VALUE_UP)
      increment(rtc_fix_interval, 1, 9999, false, mult);
    else if(key == KEY_VALUE_DOWN)
      decrement(rtc_fix_interval, 1, 9999, false, mult); 
  }
  else if(main_cursor == 1) {   // Operation
    if(key == KEY_VALUE_UP)
      increment(rtc_fix_operation, 0, 2, false, mult);
    else if(key == KEY_VALUE_DOWN)
      decrement(rtc_fix_operation, 0, 2, false, mult);
  }
}

void screen_adjust_rtc_fix__submit(){
  rtc_fix__write();
  thr_rtc_fix.setInterval((uint32_t) rtc_fix_interval * 1000);
  Display.printScroll(F("SAVE"), 1500);
}

void rtc_fix__read(){
  rtc_fix_operation = EEPROM.readInt(
    RTC_FIX_OPERATION_EEPROM_ADDRESS
  );
  
  rtc_fix_interval = EEPROM.readInt(
    RTC_FIX_INTERVAL_EEPROM_ADDRESS
  );
}

void rtc_fix__write(){
  EEPROM.writeInt(RTC_FIX_INTERVAL_EEPROM_ADDRESS, rtc_fix_interval);
  EEPROM.writeInt(RTC_FIX_OPERATION_EEPROM_ADDRESS, rtc_fix_operation);
}


/*    *    *    *    THREAD    *    *    *    */

void thr_main_func() {
  if(debug_mode){
    // PinMode display digits pins
    PORTD = 0b00000010;                   // Set all PortD pins to HIGH
    PORTB = 0b00111100;              // Set all PortB pins to HIGH
    return;
  }

  Display.setTimeSeparator(false);

  long m = millis();
  bool idle = ir.isIdle(1000) && panel.isIdle(1000);
  main_cursor_blink = ir.isIdle(500) && panel.isIdle(500);
  
  switch(main_current_screen){
    case MAIN_SCREEN_HOME:
      thr_main.setInterval(MAIN_INTERVAL);
    
      // Initial interval
      if(m <= 2000){ /* Do nothing */ } else
      
      // Temperature                  // Each 20s, runs on 16s, per 2s
      if(((m / 2000 % 10 == 8 && main_cursor == 0 && idle) || main_cursor == 1) && dht_temp_buffer.getAverage() != DHT_INIT_VALUE){
        Display.setCursor(0);
        Display.print((int) dht_temp_buffer.getAverage());
        Display.printEnd(F("*C"));
      } else
      
      // Humidity                     // Each 20s, runs on 18s, per 2s
      if(((m / 2000 % 10 == 9 && main_cursor == 0 && idle) || main_cursor == 2) && dht_hum_buffer.getAverage() != DHT_INIT_VALUE){
        Display.clear();
        Display.setCursor(0);
        Display.print(F("H"));
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
      Display.print(F("BR"));

      if(Display.getBrightness() < 10)
        Display.print(F(" "));
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
          Display.print(' ');
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
    
      if(main_cursor == 0 || main_cursor == 1){  // Minutes & Hours
        Display.setCursor(0);

        if((main_cursor == 1 && millis()/DISP_BRINK_INTERVAL % 3 == 0) && main_cursor_blink){   // Blink 1/3 on focus
          Display.print(' ');
          Display.print(' ');
        }
        else {
          if(time_adjust_hour < 10)
            Display.print(0);
          Display.print(time_adjust_hour);
        }

        if((main_cursor == 0 && millis()/DISP_BRINK_INTERVAL % 3 == 0) && main_cursor_blink){   // Blink 1/3 on focus
          Display.print(' ');
          Display.print(' ');
        }
        else {
          if(time_adjust_minute < 10)
            Display.print(0);
          Display.print(time_adjust_minute);
        }
      }
      else
      if(main_cursor == 2){  // Day
        Display.setCursor(0);
        Display.print(F("D "));

        if((millis()/DISP_BRINK_INTERVAL % 3 == 0) && main_cursor_blink){  // Blink 1/3 on focus
          Display.print(F("  "));
        }
        else {
          if(time_adjust_day % 100 < 10)
            Display.print(F(" "));
            
          Display.printEnd(time_adjust_day);
        }
      }
      else
      if(main_cursor == 3){  // Month
        Display.setCursor(0);
        Display.print(F("M "));

        if((millis()/DISP_BRINK_INTERVAL % 3 == 0) && main_cursor_blink){  // Blink 1/3 on focus
          Display.print(F("  "));
        }
        else {
          if(time_adjust_month % 100 < 10)
            Display.print(F(" "));
            
          Display.printEnd(time_adjust_month);
        }
      }
      else
      if(main_cursor == 4){  // Year
        Display.setCursor(0);
        Display.print(F("Y "));

        if((millis()/DISP_BRINK_INTERVAL % 3 == 0) && main_cursor_blink){  // Blink 1/3 on focus
          Display.print(F("  "));
        }
        else {
          if(time_adjust_year % 100 < 10)
            Display.print(F(" "));
            
          Display.printEnd((int) time_adjust_year % 100);
        }
      }
      break;

    case MAIN_SCREEN_ADJUST_RTC_FIX:
      if(main_cursor == 0){  // Interval
        Display.clear();
        Display.setCursor(0);
        
        if(!((millis()/DISP_BRINK_INTERVAL % 3 == 0) && main_cursor_blink)){  // Blink 1/3 on focus
          Display.printEnd(rtc_fix_interval);
        }
      }
      else
      if(main_cursor == 1){ // Opetarion
        Display.setCursor(0);
        Display.print(F("OP "));
        
        if((millis()/DISP_BRINK_INTERVAL % 3 == 0) && main_cursor_blink){  // Blink 1/3 on focus
          Display.print(F(" "));
        }
        else {
          Display.printEnd(rtc_fix_operation);
        }
      }
      break;

    default:
      Display.clear();
      Display.setCursor(0);
      Display.print(F("MERR"));
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
  if(!debug_mode) Display.disable();
  float dht_temp_read = dht.readTemperature();
  float dht_hum_read = dht.readHumidity();
  if(!debug_mode) Display.enable();

  // Ignore read errors
  if(isnan(dht_temp_read) || isnan(dht_hum_read)) return;

  // Avoid first read problems
  if(dht_temp_buffer.empty()) dht_temp_buffer.fill((int) dht_temp_read);
  else                        dht_temp_buffer.insert((int) dht_temp_read);

  if(dht_hum_buffer.empty())  dht_hum_buffer.fill((int) dht_hum_read);
  else                        dht_hum_buffer.insert((int) dht_hum_read);

  if(debug_mode){
    Serial.print(F("DHT sensor: T "));
    Serial.print((int) dht_temp_read);
    Serial.print(F(", H "));
    Serial.println((int) dht_hum_read);
  }
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


void debug_mode_print_key_name(int key){
  switch(key){
    case KEY_HOME: Serial.print(F("'home'")); break;
    case KEY_VALUE_UP: Serial.print(F("'value up'")); break;
    case KEY_VALUE_DOWN: Serial.print(F("'value down'")); break;
    case KEY_FUNC_LEFT: Serial.print(F("'func left'")); break;
    case KEY_FUNC_RIGHT: Serial.print(F("'func right'")); break;
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
    Display.printScroll(F("AUTO"), 1500);
  thr_ldr.enabled = true; 
}
