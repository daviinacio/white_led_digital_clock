/*
 * (c) daviapps 2019
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

#define CHRONOMETER_INTERVAL 1000

#define RTC_INTERVAL 4000

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


// Binary data
byte seven_seg_numbers [10] = {
  0b11111100, // 0
  0b01100000, // 1
  0b11011010, // 2
  0b11110010, // 3
  0b01100110, // 4
  0b10110110, // 5
  0b10111110, // 6
  0b11100100, // 7
  0b11111110, // 8
  0b11110110  // 9
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
//Thread thr_panel = Thread();

// Display
byte disp_content [DISP_LENGTH];

Buffer disp_brightness_buffer(DISP_BR_BUFFER_LENGTH, DISP_BR_MAX);

int disp_count = 0; 
int disp_digit = 0;

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

// Chronometer
int chronometer_counter = 0;

// Buzzer
int buzzer_status = BZ_STATUS_OFF;

// DEBUG
int debug_thread_loop_tester = 0;

void setup() {
  /*    *    *    PIN MODE   *    *    */
  
  // PinMode display digits pins
  DDRD = 0xff;                    // Set all PortD pins to output (0 - 7)
  PORTD = 0xff;                   // Set all PortD pins to HIGH

  // PinMode display select pins
  DDRB = B00111110;               // Set pins 9, 10, 11, 12, 13 to output and others to input
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
  
  /*    *  LIBRARY BEGINNERS  *    */

  Wire.begin();
  rtc.begin();
  dht.setup(DHT_PIN, DHT_TYPE);
  ir_recv.enableIRIn();

  /*    *   CHECK COMPONENTS  *    */

  if (! rtc.isrunning()) {
    // following line sets the RTC to the date & time this sketch was compiled
    //rtc.adjust(DateTime(__DATE__, __TIME__));

    //_disp_brightness.fill(DISP_BR_MAX);

    while(1){
      if(millis()/1000 % 3 >= 1){
        disp_content[0] = 0b10011110; // e
        disp_content[1] = 0b00001010; // r
        disp_content[2] = 0b00001010; // r
        disp_content[3] = 0b00111010; // o
      } else {
        disp_content[0] = 0x00;
        disp_content[1] = 0x00;
        disp_content[2] = 0x00;
        disp_content[3] = 0x00;
      }
    }
  }

  /*    *   THREAD FIRST RUN  *    */
  thr_ldr_func();
  thr_rtc_func();
  thr_dht_func();

  /*    *     END OF BOOT     *    */

  disp_content[0] = 0b01111010; // d
  disp_content[1] = 0b11101110; // a
  disp_content[2] = 0b01111100; // v
  disp_content[3] = 0b00001100; // i

  //thr_ldr.enabled = false;
  //disp_brightness = DISP_BR_MAX;
}

void loop() {
  // Run the main thread controller
  cpu.run();

  // DEBUG
//  int toshow = disp_brightness;
//
//  disp_content[0] = ((toshow / 1000) % 10) > 0 || toshow >= 10000 ? seven_seg_numbers[(toshow / 1000) % 10] : 0x00;
//  disp_content[1] = ((toshow / 100) % 10) > 0 || toshow >= 1000 ? seven_seg_numbers[(toshow / 100) % 10] : 0x00;
//  disp_content[2] = ((toshow / 10) % 10) > 0 || toshow >= 100 ? seven_seg_numbers[(toshow / 10) % 10] : 0x00;;
//  disp_content[3] = seven_seg_numbers[(toshow       ) % 10];

}

/*    *    *    *    THREAD    *    *    *    */

void thr_main_func() {
  long m = millis();
  
  switch(main_current_screen){
    case MAIN_SCREEN_HOME:
      // Initial interval
      if(m <= 2000){ /* Do nothing */ } else
      
      // Temperature
      if(m / 2000 % 10 == 8 && dht_temp_buffer.getAverage() != DHT_INIT_VALUE){           // Each 20s, runs on 16s, per 2s
        disp_content[0] = seven_seg_numbers[((int) dht_temp_buffer.getAverage()) / 10];   // 0 - 9
        disp_content[1] = seven_seg_numbers[((int) dht_temp_buffer.getAverage()) % 10];   // 0 - 9
        disp_content[2] = 0b11000110;                                 // °
        disp_content[3] = 0b10011100;                                 // C
      } else
      
      // Humidity
      if(m / 2000 % 10 == 9 && dht_hum_buffer.getAverage() != DHT_INIT_VALUE){            // Each 20s, runs on 18s, per 2s
        disp_content[0] = 0b01101110;                                 // H
        disp_content[1] = 0x00;                                       // null
        disp_content[2] = seven_seg_numbers[((int) dht_hum_buffer.getAverage()) / 10];    // 0 - 9
        disp_content[3] = seven_seg_numbers[((int) dht_hum_buffer.getAverage()) % 10];    // 0 - 9
      }
      
      // Hours and Minutes
      else {                                                          // Runs when others 'IFs' are false
        disp_content[0] = seven_seg_numbers[rtc_now.hour() / 10];     // 0 - 9
        disp_content[1] = seven_seg_numbers[rtc_now.hour() % 10];     // 0 - 9
        disp_content[2] = seven_seg_numbers[rtc_now.minute() / 10];   // 0 - 9
        disp_content[3] = seven_seg_numbers[rtc_now.minute() % 10];   // 0 - 9
      }
      break;

    case MAIN_SCREEN_LDR:
      disp_content[0] = 0b00111110;   // b
      disp_content[1] = 0b00001010;   // r
      disp_content[2] = ((disp_brightness_buffer.getAverage() / 10) % 10) > 0 || disp_brightness_buffer.getAverage() >= 100 ? seven_seg_numbers[(disp_brightness_buffer.getAverage() / 10) % 10] : 0x00;
      disp_content[3] = ((disp_brightness_buffer.getAverage() / 1) % 10) > 0 || disp_brightness_buffer.getAverage() >= 10 ? seven_seg_numbers[(disp_brightness_buffer.getAverage() / 1) % 10] : 0x00;
      break;

    case MAIN_SCREEN_CHRONOMETER:
      // Put the last digits of number on the display content array
      for(int i = 0; i < DISP_LENGTH; i++)
        disp_content[i] = ((chronometer_counter / (int) pow(10, DISP_LENGTH - i - 1)) % 10) > 0 || chronometer_counter >= (int) pow(10, DISP_LENGTH - i) || (chronometer_counter == 0 && i == DISP_LENGTH - 1) ? seven_seg_numbers[(chronometer_counter / (int) pow(10, DISP_LENGTH - i - 1)) % 10] : 0x00;
        break;

      default:
        disp_content[0] = 0b11101100; // m
        disp_content[1] = 0b10011110; // e
        disp_content[2] = 0b00001010; // r
        disp_content[3] = 0b00001010; // r
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
  dht_temp_buffer.insert(dht_temp_read);
  dht_hum_buffer.insert(dht_hum_read);
}

void thr_ir_func(){
  //for (int read_try = 0; ir_recv.decode(&ir_results) and read_try < IR_TRYOUT; read_try++) {
  if(ir_recv.decode(&ir_results)){
    // Read the ir HEX value
    unsigned long value = ir_results.value;
    // Receive the next value
    ir_recv.resume();

    // Temporary variable
    float tp;

    switch(value){
      case 0xC26BF044: buzzer_status = BZ_STATUS_OLDCLOCK; break; // Up
      case 0xC4FFB646:  break; // Down
      case 0x758C9D82:  break; // Left
      case 0x53801EE8:  break; // Right
      case 0x8AF13528: buzzer_status = BZ_STATUS_OFF; break; // Center
      
      case 0x3BCD58C8: main_current_screen = MAIN_SCREEN_HOME; break; // Return
      case 0x974F362:  main_current_screen = MAIN_SCREEN_HOME; break; // Exit
      
      case 0x68733A46:  //disp_brightness += DISP_BR_MAX/8; thr_ldr.enabled = false; 
        thr_ldr.enabled = false;
        
        tp = disp_brightness_buffer.getAverage();
        tp += DISP_BR_MAX/8;
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
        
      case 0x2340B922:  thr_ldr.enabled = true; break; // Mute
      
      case 0x6BDD79E6:  break; // Content
  
      case 0xDAEA83EC:  main_current_screen = MAIN_SCREEN_LDR; break; // A
      case 0x2BAFCEEC:  main_current_screen = MAIN_SCREEN_CHRONOMETER; break; // B
      case 0xB5210DA6:  break; // C
      case 0x71A1FE88:  break; // D
  
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

/*    *    *    *    TIMER2    *    *    *    */
ISR(TIMER2_COMPA_vect){
  // Clean display
  PORTD = 0xff;                                   // Sets all PORTD pins to HIGH

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

void DispTimer_enable(){
  TIMSK2 |= (1 << OCIE2A); // enable timer compare interrupt
}

void DispTimer_disable(){
  TIMSK2 &= ~(1 << OCIE2A); // disable timer compare interrupt
  PORTD = 0xff;             // Clean display
}
