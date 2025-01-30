#include "Queue.h"
Queue q;
void setup() {
  Serial.begin(115200);
  // put your setup code here, to run once:
  for (long i = 0; i < 100000; i++)
  {
    q.add(i % 256);
  }
  for (long i = 0; i < 1000; i++)
  {
    Serial.println(q.pop()); 
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
