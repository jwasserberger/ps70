#include <Arduino.h>
#include "Queue.h"

Queue::Queue(){
  f = r = 0;
  currSize = 0; 
}
void Queue::add(uint8_t value)
{
    // Serial.println(value);
    if(this->currSize == MAX_SIZE)
    {
      return;
    }
    this->arr[r] = value;
    r = (r + 1) % MAX_SIZE;
    this->currSize++;
}
 
// Function to delete element from Circular Queue
uint8_t Queue::pop()
{
    if(this->currSize==0)
    {
      return 142;
    }
    uint8_t popVal = this->arr[f];
    f = (f+1)%MAX_SIZE;
    this->currSize--;
    return popVal;
}

bool Queue::isFull()
{
  return this->currSize == this->MAX_SIZE;
}

bool Queue::isEmpty()
{
  return this->currSize == 0;
}
// Function displaying the elements
// of Circular Queue
void Queue::print()
{
    int cursor = f;
    while (cursor != r){
      Serial.printf("%d, ", this->arr[cursor]);
      cursor = (cursor+1) % MAX_SIZE;
    }
    Serial.println();
}