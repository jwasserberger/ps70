#include <ESP_I2S.h>
#include <stdint.h>
I2SClass I2S;

hw_timer_t *timer = NULL;
unsigned int sampleRate = 16000;
const int dacPin = 26;
const int buttonPin = 19;
const int volControl = 39;
uint8_t volume = 1;
const uint8_t maxVolume = 2; //the mulitplier of the default volume


int32_t range = 128000;

volatile long i = 0;

const long MAX_SOUND_SIZE = 50000;
long soundLength = 0;
uint8_t tempSound[MAX_SOUND_SIZE];

void ARDUINO_ISR_ATTR onTimer() {
  // if (digitalRead(buttonPin) == LOW)
  // {
  //   return;
  // }
  if (soundLength == 0)
  {
    return;
  }
  if (i == MAX_SOUND_SIZE)
  {
    i = 0;
    soundLength = 0;
  }
  // volume = map (analogRead(volControl)), 0, 4095, 0 , 2);
  dacWrite(26, min((unsigned long)255,(unsigned long)(tempSound[i]*(analogRead(volControl)/4095.0) * maxVolume)));
  i++;
}

void setup() {
  // Open serial communications and wait for port to open:
  // A baud rate of 115200 is used instead of 9600 for a faster data rate
  // on non-native USB ports
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  I2S.setPins(32, 25, -1, 33, -1); //SCK, WS, SDOUT, SDIN, MCLK
  // start I2S at 16 kHz with 32-bits per sample
  if (!I2S.begin(I2S_MODE_STD, 16000, I2S_DATA_BIT_WIDTH_32BIT, I2S_SLOT_MODE_MONO)) {
    Serial.println("Failed to initialize I2S!");
    while (1); // do nothing
  }

  pinMode(dacPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  // Set timer frequency to 1Mhz
  timer = timerBegin(1000000);

  // Attach onTimer function to our timer.
  timerAttachInterrupt(timer, &onTimer);

  // Set alarm to call onTimer function every second (value in microseconds).
  // Repeat the alarm (third parameter) with unlimited count = 0 (fourth parameter).
  timerAlarm(timer, 1000000/sampleRate, true, 0);
}

void loop() {
  // if (digitalRead(buttonPin) == LOW)
  // {
    // read a sample
    int sample = I2S.read();

    if ((sample == 0) || (sample == -1) ) {
      return;
    }
    // convert to 18 bit signed
    sample >>= 14; 
    // Serial.print(256);
    // Serial.print(" ");
    // Serial.print(0);
    // Serial.print(" ");
    uint32_t normalizedSample = max(-range, min(range,(int32_t) sample + 14800)) + range;
    // uint8_t shiftedSample = normalizedSample >> 10;
    // uint32_t unsignedSample = (sample + UINT32_MAX + 1);
    // uint8_t shiftedSample = unsignedSample >> 10;
    // if it's non-zero print value to serial
    tempSound[soundLength] = normalizedSample/1000;
    // Serial.println(normalizedSample/1000);
    soundLength++;
    // Serial.println(normalizedSample / 100);
  // }
  
}
