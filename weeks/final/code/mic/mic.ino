#include <ESP_I2S.h>
#include <stdint.h>
#include <Queue.h>
I2SClass I2S;

Queue q;
// Interupt Timer
hw_timer_t *timer = NULL;

// Microphone Sample Rate
unsigned int sampleRate = 16000;

// Pin Mapping
const int dacPin = 26;
const int buttonPin = 19;

// Define volume control pot connection
// ADC3 is GPIO 39
const int volControl = 39;
uint8_t volume = 1;
const uint8_t maxVolume = 2; //the mulitplier of the default volume
// Range of Audio Signal Volumes, Peak Signed Amplitudes
int32_t range = 128000;

// Increment Value
volatile long i = 0;    

// Sound Sampling Array Definition
const long MAX_SOUND_SIZE = 50000;
long soundLength = 0;
uint8_t tempSound[MAX_SOUND_SIZE];
volatile uint8_t outputVal = 0;
volatile uint8_t lastVal = 0;
volatile bool dumpBuffer = false;

// Play Audio Function for Interrupt
void ARDUINO_ISR_ATTR onTimer() {

    outputVal = q.pop();
  // Play Audio
  dacWrite(26, outputVal);

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

  pinMode(dacPin, OUTPUT*volume);
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
  // if (!dumpBuffer)
  // {
    // read a sample
    int sample = I2S.read();

    if ((sample == 0) || (sample == -1) ) {
      return;
    }
    // convert to 18 bit signed
    sample >>= 14; 

    uint32_t normalizedSample = max(-range, min(range,(int32_t) sample + 14800)) + range;

    q.add(normalizedSample/1000);

  // }
    
  
}
