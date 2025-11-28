#include <Arduino.h>
#include <Thread.h>
#include <avr/io.h>
#include <DHT.h>

#include "lib/Buffer.h"
#include "Display.h"

#define DHT_INIT_VALUE -255
#define DHT_PIN A0
#define DHT_TYPE DHT11
#define DHT_BUFFER_LENGTH 4
#define DHT_BUFFER_INTERVAL 60000

#ifndef WLDC_DHT_DRIVER_H
#define WLDC_DHT_DRIVER_H

class DhtDriver : public Thread {
protected:
  DHT dht;
  Buffer temp_buffer;
  Buffer hum_buffer;

public:
  DhtDriver():
    dht(DHT_PIN, DHT_TYPE),
    temp_buffer(DHT_BUFFER_LENGTH, DHT_INIT_VALUE),
    hum_buffer(DHT_BUFFER_LENGTH, DHT_INIT_VALUE) {
      enabled = false;
      interval = DHT_BUFFER_INTERVAL / DHT_BUFFER_LENGTH;
  };

  void begin(){
    dht.begin();
    enabled = true;
  };

  void run(){
    // Disable display timer while dht read
    display.disable();
    cli();
    float dht_temp_read = dht.readTemperature();
    float dht_hum_read = dht.readHumidity();
    sei();
    display.enable();

    // Ignore read errors
    if(isnan(dht_temp_read) || isnan(dht_hum_read)) return;

    // Avoid first read problems
    if(temp_buffer.empty()) temp_buffer.fill((int) dht_temp_read);
    else                        temp_buffer.insert((int) dht_temp_read);

    if(hum_buffer.empty())  hum_buffer.fill((int) dht_hum_read);
    else                        hum_buffer.insert((int) dht_hum_read);

    Thread::run();
  };

  int getTemperature(){
    return temp_buffer.getAverage();
  }

  bool hasTemperature(){
    return getTemperature() != DHT_INIT_VALUE;
  }

  int getHumidity(){
    return hum_buffer.getAverage();
  }

  bool hasHumidity(){
    return getHumidity() != DHT_INIT_VALUE;
  }
};

DhtDriver dht;

#endif
