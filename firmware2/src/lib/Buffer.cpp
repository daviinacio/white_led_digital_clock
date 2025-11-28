#include "Buffer.h"

// Constructors
Buffer::Buffer(unsigned short length, short init){
  this->values = new short[length];
  this->length = length;
  this->init = init;
  this->clear();
}

// Getters
short Buffer::calcAverage(){
  float sum = 0;
  
  for(unsigned short i = 0; i < this->length; i++)
    sum += this->values[i];
    
  this->average = sum / this->length;
    
  return this->average;
}

short Buffer::getAverage(){
  return this->average;
}

short Buffer::getAt(unsigned short position){
  return this->values[position];
}

bool Buffer::empty(){
  return this->calcAverage() == this->init;  
}

unsigned short Buffer::size(){
  return this->length;
}

// Setters
void Buffer::fill(short value){
  for(unsigned short i = 0; i < this->length; i++)
    this->values[i] = value;
    
  this->calcAverage();
}

void Buffer::insert(short value){
  // Move array values
  for(unsigned short i = this->length -1; i > 0; i--)
    this->values[i] = this->values[i - 1];
  
  // Insert new value
  this->values[0] = value;
  
  // Update the average variable
  this->calcAverage();
}

void Buffer::clear(){
  // Clear the Buffer and enable to fill on first insert
  this->fill(this->init);
}
