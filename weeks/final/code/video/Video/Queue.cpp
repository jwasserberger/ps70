#include <Arduino.h>
#include "Queue.h"

Queue::Queue(){
  f = r = 0;
  currSize = 0; 
}
void Queue::add(int8_t value)
{
    // Serial.println(value);
    if(this->currSize == MAX_SIZE)
    {
      return;
    }
    this->arr[r] = (int16_t)value - 128;
    r = (r + 1) % MAX_SIZE;
    this->currSize++;
}
 
// Function to delete element from Circular Queue
uint8_t Queue::pop()
{
    if(this->currSize==0)
    {
      return 0;
    }
    uint8_t popVal = this->arr[f];
    uint8_t delayVal = this->delayArr[f];
    this->delayArr[f] = 0;
    f = (f+1)%MAX_SIZE;
    
    this->currSize--;
    return min(255, popVal + delayVal);
}

// Test
// Function to add delay value
void Queue::delay(int8_t currSound)
{
  delayArr[(f + 6000) % MAX_SIZE] = currSound/3;
    // if(this->currSize > 10000)
    // {
    //   //First Wave - Half Volume
    //   this->arr[(f + 10000)%MAX_SIZE] += 0;
    // }

    

    // // Second Wave = Quarter Volume
    // delayVal = (delayVal - 128)/2;

}

// uint8_t Queue::sine()
// {

// }


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