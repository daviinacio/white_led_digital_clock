#ifndef WLDC_BUFFER_H
#define WLDC_BUFFER_H

class Buffer {
  public:
    // Constructors
    Buffer(unsigned short, short);
    
    // Getters    
    short calcAverage();
    short getAverage();
    short getAt(unsigned short);
    bool empty();
    unsigned short size();
    
    // Setters
    void fill(short);
    void insert(short);
    void clear();
    
  private:
    short init;
    short *values;
    unsigned short length;
    short average;
};

#endif
