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

#define MAIN_INTERVAL 1000
//#define MAIN_SCREEN_LENGTH 
#define MAIN_SCREEN_HOME 0
#define MAIN_SCREEN_LDR 1

#define RTC_INTERVAL 4000

#define DHT_PIN A0
#define DHT_TYPE DHT11
#define DHT_BUFFER_LENGTH 6
#define DHT_BUFFER_INTERVAL 60000

#define IR_PIN A1
#define IR_INTERVAL 100

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

// Threads
ThreadController cpu = ThreadController();
Thread thr_main = Thread();
Thread thr_ldr = Thread();
Thread thr_rtc = Thread();
Thread thr_dht = Thread();
Thread thr_ir = Thread();
//Thread thr_panel = Thread();

// Display
byte disp_content [DISP_LENGTH];

int disp_brightness = DISP_BR_MIN;
int disp_brightness_buffer[DISP_BR_BUFFER_LENGTH];

int disp_count = 0; 
int disp_digit = 0;

// RTC
RTC_DS1307 rtc;
DateTime rtc_now;

// DHT
DHT dht;

float dht_temp = 0;
float dht_hum = 0;

int dht_temp_buffer[DHT_BUFFER_LENGTH];
int dht_hum_buffer[DHT_BUFFER_LENGTH];

// IR Remore
IRrecv ir_recv(IR_PIN);
decode_results ir_results;

int c_ir_digit = 0;

// Main
int main_current_screen = 0;

// DEBUG
int debug_thread_loop_tester = 0;

void setup() {
  // Restart the TIMER2
  TCNT2 = 0;

  /*    *    *    PIN MODE   *    *    */
  
  // PinMode display digits pins
  DDRD = 0xff;                    // Set all PortD pins to output (0 - 7)
  PORTD = 0xff;                   // Set all PortD pins to HIGH

  // PinMode display select pins
  DDRB = B00111100;               // Set pins 10, 11, 12, 13 to output and others to input
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
  OCR2A = 128;
  
  // Enable to TIMER2 Interrupt
  TIMSK2 |= (1 << OCIE2A);

  /*    *    *  THREADS  *    *    */

  // Initialize threads
  thr_main.onRun(thr_main_func);
  thr_main.setInterval(MAIN_INTERVAL);

  thr_ldr.onRun(thr_ldr_func);
  thr_ldr.setInterval(DISP_BR_BUFFER_INTERVAL / DISP_BR_BUFFER_LENGTH);

  thr_rtc.onRun(thr_rtc_func);
  thr_rtc.setInterval(RTC_INTERVAL);

  thr_dht.onRun(thr_dht_func);
  thr_dht.setInterval(DHT_BUFFER_INTERVAL / DHT_BUFFER_LENGTH);

  thr_ir.onRun(thr_ir_func);
  thr_ir.setInterval(IR_INTERVAL);

  //thr_panel.onRun(thread_panel_loop);
  //thr_panel.setInterval(1000);


  // Add thread to thead controll
  cpu.add(&thr_main);
  cpu.add(&thr_ldr);
  cpu.add(&thr_rtc);
  cpu.add(&thr_dht);
  cpu.add(&thr_ir);
  
  /*    *  LIBRARY BEGINNERS  *    */

  Wire.begin();
  rtc.begin();
  dht.setup(DHT_PIN);
  ir_recv.enableIRIn();

  /*    *   CHECK COMPONENTS  *    */

  if (! rtc.isrunning()) {
    // following line sets the RTC to the date & time this sketch was compiled
    //rtc.adjust(DateTime(__DATE__, __TIME__));

    disp_brightness = DISP_BR_MAX;

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
//  debug_thread_loop_tester++;
}

void thr_ldr_func(){
  // Make the ldr sensor read
  int br_read = map(analogRead(LDR_PIN), 0, 650, DISP_BR_MIN, DISP_BR_MAX);

  // Avoid out of range problems
  if(br_read > DISP_BR_MAX) br_read = DISP_BR_MAX; else
  if(br_read < DISP_BR_MIN) br_read = DISP_BR_MIN;

  // Avoid first read problems
  if(disp_br_average_calc() == 0)
    for(int i = 0; i < DISP_BR_BUFFER_LENGTH; i++)
      disp_brightness_buffer[i] = br_read;

  // Move the buffer to side
  for(int i = DISP_BR_BUFFER_LENGTH - 2; i >= 0; i--)
      disp_brightness_buffer[i + 1] = disp_brightness_buffer[i];
      
  // Insert the new value on buffer
  disp_brightness_buffer[0] = br_read;

  // Put the buffer average to the brightness variable
  disp_brightness = disp_br_average_calc();
}

void thr_rtc_func(){
  rtc_now = rtc.now(); 

//  disp_content[0] = seven_seg_numbers[rtc_now.hour() / 10];
//  disp_content[1] = seven_seg_numbers[rtc_now.hour() % 10];
//  disp_content[2] = seven_seg_numbers[rtc_now.minute() / 10];
//  disp_content[3] = seven_seg_numbers[rtc_now.minute() % 10];
}

void thr_dht_func(){
  T2_disable();
  float dht_temp_read = dht.getTemperature();
  float dht_hum_read = dht.getHumidity();
  T2_enable();

  // Avoid first read problems
  if(dht_temp_average_calc() == 0)
    for(int i = 0; i < DHT_BUFFER_LENGTH; i++)
      dht_temp_buffer[i] = dht_temp_read;

  if(dht_hum_average_calc() == 0)
    for(int i = 0; i < DHT_BUFFER_LENGTH; i++)
      dht_hum_buffer[i] = dht_hum_read;

  // Move the buffer to side
  for(int i = DHT_BUFFER_LENGTH - 2; i >= 0; i--)
      dht_temp_buffer[i + 1] = dht_temp_buffer[i];

  for(int i = DHT_BUFFER_LENGTH - 2; i >= 0; i--)
      dht_hum_buffer[i + 1] = dht_hum_buffer[i];
      
  // Insert the new value on buffer
  dht_temp_buffer[0] = dht_temp_read;
  dht_hum_buffer[0] = dht_hum_read;

  // Put the buffer average to the brightness variable
  dht_temp = dht_temp_average_calc();
  dht_hum = dht_hum_average_calc();


//  disp_content[0] = 0b01101110;               // H
//  disp_content[1] = 0x00;                     // 
//  disp_content[2] = seven_seg_numbers[((int) dht_hum) / 10];  // 0 - 9
//  disp_content[3] = seven_seg_numbers[((int) dht_hum) % 10];  // 0 - 9

  //disp_content[0] = seven_seg_numbers[((int) dht_temp) / 10];  // 0 - 9
  //disp_content[1] = seven_seg_numbers[((int) dht_temp) % 10];  // 0 - 9
  //disp_content[2] = 0b11000110;                 // °
  //disp_content[3] = 0b10011100;                 // C
  
}

void thr_ir_func(){
  if (ir_recv.decode(&ir_results)) {

    byte disp_content_old[DISP_LENGTH];
    for(int i = 0; i < DISP_LENGTH; i++)
      disp_content_old[i] = disp_content[i];
      

    for(int i = DISP_LENGTH - 2; i >= 0; i--)
      disp_content[i + 1] = disp_content[i];
    
    switch(ir_results.value){
      case 0xB9F56762: disp_content[0] = seven_seg_numbers[0]; break; // 0
      case 0xE13DDA28: disp_content[0] = seven_seg_numbers[1]; break; // 1
      case 0xAD586662: disp_content[0] = seven_seg_numbers[2]; break; // 2
      case 0x273009C4: disp_content[0] = seven_seg_numbers[3]; break; // 3
      case 0xF5999288: disp_content[0] = seven_seg_numbers[4]; break; // 4
      case 0x731A3E02: disp_content[0] = seven_seg_numbers[5]; break; // 5
      case 0x2C452C6C: disp_content[0] = seven_seg_numbers[6]; break; // 6
      case 0x4592E14C: disp_content[0] = seven_seg_numbers[7]; break; // 7
      case 0x6825E53E: disp_content[0] = seven_seg_numbers[8]; break; // 8
      case 0x8B8510E8: disp_content[0] = seven_seg_numbers[9]; break; // 9
      
      case 0x52A5A66: disp_content[0] = 0x00; break; // Clear
      //case 0xCA8CBCC6: c_ir_digit = 3; break; // Back to home

      case 0xCA8CBCC6:
        disp_content[0] = 0b01111010; // d
        disp_content[1] = 0b11101110; // a
        disp_content[2] = 0b01111100; // v
        disp_content[3] = 0b00001100; // i
        break;

      default:
        for(int i = 0; i < DISP_LENGTH; i++)
          disp_content[i] = disp_content_old[i];
        break;
    };

    switch(ir_results.value){
      case 0x68733A46: disp_brightness += DISP_BR_MAX/8; thr_ldr.enabled = false; break; // Brightness ++
      case 0x83B19366: disp_brightness -= DISP_BR_MAX/8; thr_ldr.enabled = false; break; // Brightness --
    }

    //if(disp_brightness > DISP_BR_MAX) disp_brightness = DISP_BR_MAX; else
    //if(disp_brightness < 8) disp_brightness = 8;

    //c_ir_digit++;
    //c_ir_digit %= DISP_LENGTH;

    ir_recv.resume(); // Receive the next value
  }
}

/*    *    * INPUT BUFFER AVERAGES *    *    */

int disp_br_average_calc(){
  return buffer_average_calc(disp_brightness_buffer, DISP_BR_BUFFER_LENGTH);
}

float dht_temp_average_calc(){
  return buffer_average_calc(dht_temp_buffer, DHT_BUFFER_LENGTH);
}

float dht_hum_average_calc(){
  return buffer_average_calc(dht_hum_buffer , DHT_BUFFER_LENGTH);
}

/*    *    *    *    TIMER2    *    *    *    */
ISR(TIMER2_COMPA_vect){

  // Clean display
  PORTD = 0xff;                                   // Sets all PORTD pins to HIGH

  // Brightness control
  if((disp_count/DISP_LENGTH) < disp_brightness){
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

void T2_enable(){
  TIMSK2 |= (1 << OCIE2A); // enable timer compare interrupt
}

void T2_disable(){
  TIMSK2 &= ~(1 << OCIE2A); // disable timer compare interrupt
  PORTD = 0xff;             // Clean display
}


/*    *    *    *    UTILS     *    *    *    */

int buffer_average_calc(int b[], int l){
  int sum = 0;

  // Sum all buffer value
  for(int i = 0; i < l; i++)
    sum += b[i];

  // Divide the buffer sum per buffer length
  return sum / l;
}

float buffer_average_calc(float b[], int l){
  float sum = 0;

  // Sum all buffer value
  for(int i = 0; i < l; i++)
    sum += b[i];

  // Divide the buffer sum per buffer length
  return sum / l;
}
