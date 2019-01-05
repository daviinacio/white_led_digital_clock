#include <Wire.h>
#include "RTClib.h"

#include <Thread.h>
#include <ThreadController.h>

ThreadController controll_task = ThreadController();

Thread thread_rtc = Thread();
Thread thread_display = Thread();

RTC_DS1307 RTC;
DateTime now;

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

void setup() {
  Wire.begin();
  Serial.begin(9600);
  
  RTC.begin();
  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }

  // RTC Thread config
  thread_rtc.onRun(thread_rtc_loop);
  thread_rtc.setInterval(1000);

  // Display Thread config
  thread_display.onRun(thread_display_loop);
  thread_display.setInterval(1);

  // Add thread controll
  controll_task.add(&thread_rtc);
  controll_task.add(&thread_display);

  // Read the first time
  thread_rtc_loop();
}

void loop() {
  controll_task.run();
}

/* LEVEL 1 */
void setDisplayNumber(int number){
  Wire.beginTransmission(32);
  Wire.write(seven_seg_numbers[number]);
  Wire.endTransmission();
}

void cleanDisplayNumber(){
  Wire.beginTransmission(32);
  Wire.write(0);
  Wire.endTransmission();
}

void setCurrentDisplay(int current){
  /*Wire.beginTransmission(33);
  
  switch(current){
    case 0: Wire.write(0b11111110); break;
    case 1: Wire.write(0b11111101); break;
    case 2: Wire.write(0b11111011); break;
    case 3: Wire.write(0b11110111); break;
    default: Wire.write(0); break;
  }
    
  Wire.endTransmission();*/
}

/* LEVEL 2 */
void thread_rtc_loop(){
  now = RTC.now(); 
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(' ');
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
}

void thread_display_loop(){
  for(int d = 0; d <= 3; d++){
    setCurrentDisplay(d);

    switch(d){
      // Minutes
      case 0: setDisplayNumber(now.minute() % 10); break;
      case 1: setDisplayNumber(now.minute() / 10); break;

      // Hours
      case 2: setDisplayNumber(now.hour() % 10); break;
      case 3: setDisplayNumber(now.hour() / 10); break;
    }

    
    delay(map(analogRead(A0), 0, 1023, 0, 250));
    
    cleanDisplayNumber();

    delay(map(analogRead(A1), 1023, 0, 0, 20));
    
    
    //delay(5);
  }
}
