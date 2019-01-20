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

#define RTC_INTERVAL 2000

#define DHT_PIN A0
#define DHT_TYPE DHT11
#define DHT_BUFFER_LENGTH 4
#define DHT_BUFFER_INTERVAL 10000

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

//#include "DHT.h"
#include <SimpleDHT.h>


// Threads
ThreadController cpu = ThreadController();
Thread thr_main = Thread();
Thread thr_ldr = Thread();
Thread thr_rtc = Thread();
Thread thr_dht = Thread();
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
//DHT dht(DHT_PIN, DHT_TYPE);
SimpleDHT11 dht(DHT_PIN);

byte dht_temp = 0x00;
byte dht_hum = 0x00;

int dht_temp_buffer[DHT_BUFFER_LENGTH];
int dht_hum_buffer[DHT_BUFFER_LENGTH];

// DEBUG
int debug_thread_loop_tester = 0;

void setup() {

  /*    *    *    PIN MODE   *    *    */
  
  // PinMode display digits pins
  DDRD = 0xff;                    // Set all PortD pins to output (0 - 7)
  PORTD = 0xff;                   // Set all PortD pins to HIGH

  // PinMode display select pins
  DDRB = B00111100;               // Set pins 10, 11, 12, 13 to output and others to input
  PORTB = 0x00;                   // Set all PortB pins to LOW

  /*    *    ATMEGA TIMER1    *    */

  // Modo de Comparação
  TCCR1A = 0;

  // Prescaler 1:256
  TCCR1B |=  (1 << CS12);
  TCCR1B &= ~(1 << CS11);
  TCCR1B &= ~(1 << CS10);

  // Inicializa Registradores
  TCNT1 = 0;
  OCR1A = 2;

  // Habilita Interrupção do Timer1
  TIMSK1 = (1 << OCIE1A);

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

  //thr_panel.onRun(thread_panel_loop);
  //thr_panel.setInterval(1000);


  // Add thread to thead controll
  cpu.add(&thr_main);
  cpu.add(&thr_ldr);
  cpu.add(&thr_rtc);
  cpu.add(&thr_dht);

  /*    *  LIBRARY BEGINNERS  *    */

  Wire.begin();
  rtc.begin();
  //dht.begin();

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
  //thr_dht_func();

  /*    *     END OF BOOT     *    */

  disp_content[0] = 0b01111010; // d
  disp_content[1] = 0b11101110; // a
  disp_content[2] = 0b01111100; // v
  disp_content[3] = 0b00001100; // i
}

void loop() {
  // Run the main thread controller
  cpu.run();

  // DEBUG
  //int toshow = dht_temp;

  //disp_content[0] = ((toshow / 1000) % 10) > 0 || toshow >= 10000 ? seven_seg_numbers[(toshow / 1000) % 10] : 0x00;
  //disp_content[1] = ((toshow / 100) % 10) > 0 || toshow >= 1000 ? seven_seg_numbers[(toshow / 100) % 10] : 0x00;
  //disp_content[2] = ((toshow / 10) % 10) > 0 || toshow >= 100 ? seven_seg_numbers[(toshow / 10) % 10] : 0x00;;
  //disp_content[3] = seven_seg_numbers[(toshow       ) % 10];

  disp_update();
}

/*    *    *    *    THREAD    *    *    *    */

void thr_main_func() {
  
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

  //disp_content[0] = seven_seg_numbers[rtc_now.hour() / 10];
  //disp_content[1] = seven_seg_numbers[rtc_now.hour() % 10];
  //disp_content[2] = seven_seg_numbers[rtc_now.minute() / 10];
  //disp_content[3] = seven_seg_numbers[rtc_now.minute() % 10];
}

void thr_dht_func(){
  //dht_temp = dht.readTemperature();

  int err = SimpleDHTErrSuccess;
  if ((err = dht.read(&dht_temp, &dht_hum, NULL)) != SimpleDHTErrSuccess) {
    disp_content[0] = 0b10011110; // e
    disp_content[1] = 0b00001010; // r
    disp_content[2] = seven_seg_numbers[err / 10]; // 0 - 9
    disp_content[3] = seven_seg_numbers[err % 10]; // 0 - 9
    return;
  }

  disp_content[0] = seven_seg_numbers[((int) dht_temp) / 10];  // 0 - 9
  disp_content[1] = seven_seg_numbers[((int) dht_temp) % 10];  // 0 - 9
  disp_content[2] = 0b11000110;                 // °
  disp_content[3] = 0b10011100;                 // C
  
  //Serial.print((int)temperature); Serial.print(" *C, "); 
  //Serial.print((int)humidity); Serial.println(" H");
  
}

/*    *    * INPUT BUFFER AVERAGES *    *    */

int disp_br_average_calc(){
  return buffer_average_calc(disp_brightness_buffer, DISP_BR_BUFFER_LENGTH);
}

int dht_temp_average_calc(){
  //return buffer_average_calc( , );
}

int dht_hum_average_calc(){
  //return buffer_average_calc( , );
}

/*    *    *    *    TIMER1    *    *    *    */

void disp_update(){
//ISR(TIMER1_COMPA_vect) {
}

ISR(TIMER1_COMPA_vect) {
//void disp_update(){

  // Restart the TIMER1
  TCNT1 = 0;

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
