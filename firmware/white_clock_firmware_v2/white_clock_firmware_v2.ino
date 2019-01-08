//#include <IRremote.h>

#include "DHT.h"
#define DHTTYPE DHT11 

#include <Wire.h>
#include "RTClib.h"

#include <Thread.h>
#include <ThreadController.h>

ThreadController controll_task = ThreadController();

Thread thread_rtc = Thread();
Thread thread_ldr = Thread();
Thread thread_ir = Thread();
Thread thread_dht = Thread();
//Thread thread_display = Thread();

int display_brightness = 0;

RTC_DS1307 RTC;
DateTime now;

//IRrecv irrecv(A1);
//decode_results results;

DHT dht(A0, DHTTYPE);

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

// Display digits uses the PORTD

int disp_sel_start = PB2;
int disp_sel_end = PB5;

byte displays_content [4] = {
  0x00, 0x00, 0x00, 0x00
};

//int display_current_pin = 0;

void setup() {
//  Wire.begin();
//  Serial.begin(9600);

  // PinMode display digits pins
  DDRD = B11111111;               // Set all PortD pins to output (0 - 7)
  PORTD = B11111111;              // Set all PortD pins to HIGH=

  // PinMode display select pins
  DDRB = B00111100;               // Set pins 10, 11, 12, 13 to output and others to input
  PORTB = B00000000;              // Set all PortB pins to LOW=
  
  // RTC.begin();
  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }

  // RTC Thread config
  thread_rtc.onRun(thread_rtc_loop);
  thread_rtc.setInterval(4000);

  // LDR Thread config
  thread_ldr.onRun(thread_ldr_read);
  thread_ldr.setInterval(100);

  // InfraRed Thread config
  thread_ir.onRun(thread_ir_received);
  thread_ir.setInterval(250);

  // DHT11 Thread config
  thread_dht.onRun(thread_dht_read);
  thread_dht.setInterval(5000);

  // Display Thread config
  //thread_display.onRun(thread_display_loop);
  //thread_display.setInterval(1);

  // Add thread controll
  controll_task.add(&thread_rtc);
  controll_task.add(&thread_ldr);
  controll_task.add(&thread_ir);
  controll_task.add(&thread_dht);
  //controll_task.add(&thread_display);

  // Timer to multiplex
  /*cli();
  TCCR2A = 0;// set entire TCCR2A register to 0
  TCCR2B = 0;// same for TCCR2B
  TCNT2  = 0;//initialize counter value to 0
  // set compare match register for 8khz increments
  OCR2A = 700;// = (16*10^6) / (8000*8) - 1 (must be <256)
  // turn on CTC mode
  TCCR2A |= (1 << WGM21);
  // Set CS21 bit for 8 prescaler
  TCCR2B |= (1 << CS21);   
  // enable timer compare interrupt
  TIMSK2 |= (1 << OCIE2A);
  sei();*/

  // Read the first time
  //thread_rtc_loop();

  //irrecv.enableIRIn(); // Start the receiver

//  dht.begin();


  //displays_content[0] = 0b01101110; // H
  //displays_content[1] = 0b10011110; // E
  //displays_content[2] = 0b00011100; // L
  //displays_content[3] = 0b00111010; // o

  //displays_content[0] = 0b00111110; // b
  //displays_content[1] = 0b00111010; // o
  //displays_content[2] = 0b00111010; // o
  //displays_content[3] = 0b01100010; // t

  displays_content[0] = 0b01111010; // d
  displays_content[1] = 0b11101110; // a
  displays_content[2] = 0b01111100; // v
  displays_content[3] = 0b00001100; // i
}

void loop() {
  controll_task.run();
  //thread_display_loop();
//  displays_content[0] = seven_seg_numbers[millis() %10];

  display_update();
}

/* Threads */
void thread_rtc_loop(){
  now = RTC.now(); 

  displays_content[0] = seven_seg_numbers[now.hour() / 10];
  displays_content[1] = seven_seg_numbers[now.hour() % 10];
  displays_content[2] = seven_seg_numbers[now.minute() / 10];
  displays_content[3] = seven_seg_numbers[now.minute() % 10];

  
  Serial.print(now.day(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.year(), DEC);
  Serial.print(' ');
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();

  //displays_content[0] = 0b10111110; // b
  //displays_content[1] = 0b10111010; // o
  //displays_content[2] = 0b10111010; // o
  //displays_content[3] = 0b11100010; // t
}

//ISR(TIMER2_COMPA_vect){
void display_update(){
  for(int dp = 0; dp <= disp_sel_end - disp_sel_start; dp++){

    // Active the current digit of display
    PORTB |= 0b00111100;                  // Puts pins PB2, PB3, PB4, PB5 to HIGH
    PORTB ^= 0b00111100;                  // Toggle pins PB2, PB3, PB4, PB5 to LOW

    PORTB ^= (1 << (disp_sel_start + dp));             // Toggle the current digit pin to HIGH

    // Set display content
    PORTD ^= displays_content[dp];        // Sets the display content and uses ^= to invert the bits 
                                          // (the display actives with LOW state)  

    // Time to stay on
    delayMicroseconds(map(display_brightness, 0, 100, 0, 1000));

    // Clean display
    PORTD = B11111111;                    // Sets all PORTD pins to HIGH

    // Time to stay off
    delayMicroseconds(map(display_brightness, 0, 100, 2500, 0));
  }
}

/*void thread_display_loop(){
  for(int dp = 0; dp < (sizeof(display_select_pins)/sizeof(*display_select_pins)); dp++){

    // Active the current digit of display
    for(int i = 0; i < (sizeof(display_select_pins)/sizeof(*display_select_pins)); i++)
      digitalWrite(display_select_pins[i], i == dp);

    // Set display content
    for(int i = 0; i < (sizeof(display_dig_pins)/sizeof(*display_dig_pins)); i++)
      digitalWrite(display_dig_pins[i], !((displays_content[dp] >> i) & 0x01));

    // Time to stay on
    delayMicroseconds(map(display_brightness, 0, 100, 0, 1000));

    // Clean display
    for(int i = 0; i < (sizeof(display_dig_pins)/sizeof(*display_dig_pins)); i++)
      digitalWrite(display_dig_pins[i], HIGH);

    // Time to stay off
    delayMicroseconds(map(display_brightness, 0, 100, 2500, 0));
  }
}*/

void thread_ldr_read(){
  display_brightness = map(analogRead(A2), 0, 1023, 0, 100);

  /*Serial.print("Display brightness: ");
  Serial.print(display_brightness);
  Serial.println('%');*/
}

int c_ir_digit = 0;

void thread_ir_received(){
  /*if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);

    

    switch(results.value){
      case 0xB9F56762: displays_content[c_ir_digit] = seven_seg_numbers[0]; break; // 0
      case 0xE13DDA28: displays_content[c_ir_digit] = seven_seg_numbers[1]; break; // 1
      case 0xAD586662: displays_content[c_ir_digit] = seven_seg_numbers[2]; break; // 2
      case 0x273009C4: displays_content[c_ir_digit] = seven_seg_numbers[3]; break; // 3
      case 0xF5999288: displays_content[c_ir_digit] = seven_seg_numbers[4]; break; // 4
      case 0x731A3E02: displays_content[c_ir_digit] = seven_seg_numbers[5]; break; // 5
      case 0x2C452C6C: displays_content[c_ir_digit] = seven_seg_numbers[6]; break; // 6
      case 0x4592E14C: displays_content[c_ir_digit] = seven_seg_numbers[7]; break; // 7
      case 0x6825E53E: displays_content[c_ir_digit] = seven_seg_numbers[8]; break; // 8
      case 0x8B8510E8: displays_content[c_ir_digit] = seven_seg_numbers[9]; break; // 9
      
      case 0x52A5A66: displays_content[c_ir_digit] = 0x00; break; // Clear
      case 0xCA8CBCC6: c_ir_digit = 3; break; // Back to home
    };

    c_ir_digit++;
    c_ir_digit = c_ir_digit % 4;
    //if(c_ir_digit > 3) c_ir_digit = 0;

    irrecv.resume(); // Receive the next value
  }*/
}

void thread_dht_read(){
  long init_time_read = millis();
  
  int t = dht.readTemperature();
  
  if(t > 0){
    Serial.print("Temperature: "); 
    Serial.print(t);
    Serial.println(" *C");
  
    displays_content[0] = seven_seg_numbers[t / 10];
    displays_content[1] = seven_seg_numbers[t % 10];
    displays_content[2] = 0b11000110; // °
    displays_content[3] = 0b10011100; // C
  
    Serial.print(millis() - init_time_read);
    Serial.println("ms");
  }
}
