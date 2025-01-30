#include <ESP_I2S.h>
#include <stdint.h>
#include <Queue.h>
I2SClass I2S;

// VIDEO DEFINITIONS

// Define meter size as 1 for tft.rotation(0) or 1.3333 for tft.rotation(1)
#define M_SIZE 1.3333

#include <TFT_eSPI.h> // Hardware-specific library
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI();       // Invoke custom library

#define TFT_GREY 0x5AEB
#define TFT_DARKCYAN 0xFDA0 //0x03EF 
#define TFT_BLACK 0xFFFF
#define TFT_SKYBLUE 0x867D


float ltx = 0;    // Saved x coord of bottom of needle
uint16_t osx = M_SIZE*120, osy = M_SIZE*120; // Saved x & y coords
uint32_t updateTime = 0;       // time for next update

int old_analog =  -999; // Value last displayed

int value[6] = {0, 0, 0, 0, 0, 0};
int old_value[6] = { -1, -1, -1, -1, -1, -1};
int d = 0;

// AUDIO DEFINITIONS

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
int32_t range = 128000; // OG
int32_t videoRange = 2000; // 
// int32_t range = 256000; // test - too loud

// Increment Value
volatile long i = 0;    

// Sound Sampling Array Definition
const long MAX_SOUND_SIZE = 50000;
long soundLength = 0;
uint8_t tempSound[MAX_SOUND_SIZE];
volatile int8_t outputVal = 0;
// volatile int8_t lastVal = 0;
// float LPF_beta = 0.01;
volatile bool dumpBuffer = false;

// Play Audio Function for Interrupt
void ARDUINO_ISR_ATTR onTimer() {

    outputVal = q.pop();
    
    // Silencing Function
    //if ((outputVal < 1) && (outputVal > -1)) {
    //  outputVal = 0;
    //}

  // Play Audio
  // dacWrite(26, min((unsigned long)255,(unsigned long)(tempSound[i]*(analogRead(volControl)/4095.0) * maxVolume)));
  
  // Add Delay
  // q.delay(outputVal);
    
  //dacWrite(26, outputVal + 128);
  dacWrite(26, outputVal);

  // }
  
  // i++;
}

void setup() {

  // AUDIO

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

  // VIDEO
  tft.init();
  tft.setRotation(1);
  // Serial.begin(57600); // For debug
  tft.fillScreen(TFT_DARKCYAN);

  analogMeter(); // Draw analogue meter

  updateTime = millis(); // Next update time
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

    uint32_t normalizedSample = max(-range, min(range,(int32_t) sample)) + range;
    //uint32_t normalizedSample = max(-range, min(range,(int32_t) sample));
    int32_t videoSample = map (sample, -videoRange, videoRange, -128 , 128);

    //q.add(normalizedSample/1000);
    q.add(normalizedSample);
    //q.add(sample);
    //Serial.println(normalizedSample/1000);
    // Serial.println(videoSample);
    // Serial.println(normSample);
  // }

  // VIDEO
  if (updateTime <= millis()) {
    updateTime = millis() + 3; // Update meter every 3 milliseconds
 
    // Create a Sine wave for testing
    d += 4; if (d >= 360) d = 0;
    //value[0] = 50 + 50 * sin((d + 0) * 0.0174532925);
 
    // plotNeedle(value[0], 0); // It takes between 2 and 12ms to replot the needle with zero delay
    plotNeedle((videoSample), 0);
  }
}

void analogMeter()
{

  // Meter outline
  tft.fillRect(0, 0, M_SIZE*320, M_SIZE*240, TFT_GREY);
  tft.fillRect(5, 3, M_SIZE*309, M_SIZE*229, TFT_DARKCYAN); //TFT_WHITE

  tft.setTextColor(TFT_BLACK);  // Text colour

  // Draw ticks every 5 degrees from -50 to +50 degrees (100 deg. FSD swing)
  for (int i = -50; i < 51; i += 5) {
    // Long scale tick length
    int tl = 15;

    // Coordinates of tick to draw
    float sx = cos((i - 90) * 0.0174532925);
    float sy = sin((i - 90) * 0.0174532925);
    uint16_t x0 = sx * (M_SIZE*100 + tl) + M_SIZE*120;
    uint16_t y0 = sy * (M_SIZE*100 + tl) + M_SIZE*140;
    uint16_t x1 = sx * M_SIZE*100 + M_SIZE*120;
    uint16_t y1 = sy * M_SIZE*100 + M_SIZE*140;

    // Coordinates of next tick for zone fill
    float sx2 = cos((i + 5 - 90) * 0.0174532925);
    float sy2 = sin((i + 5 - 90) * 0.0174532925);
    int x2 = sx2 * (M_SIZE*100 + tl) + M_SIZE*120;
    int y2 = sy2 * (M_SIZE*100 + tl) + M_SIZE*140;
    int x3 = sx2 * M_SIZE*100 + M_SIZE*120;
    int y3 = sy2 * M_SIZE*100 + M_SIZE*140;

    // Yellow zone limits
    //if (i >= -50 && i < 0) {
    //  tft.fillTriangle(x0, y0, x1, y1, x2, y2, TFT_YELLOW);
    //  tft.fillTriangle(x1, y1, x2, y2, x3, y3, TFT_YELLOW);
    //}

    // Green zone limits
    if (i >= -128 && i < -64) {
      tft.fillTriangle(x0, y0, x1, y1, x2, y2, TFT_GREEN);
      tft.fillTriangle(x1, y1, x2, y2, x3, y3, TFT_GREEN);
    }

    // Orange zone limits
    if (i >= -64 && i < 0) {
      tft.fillTriangle(x0, y0, x1, y1, x2, y2, TFT_ORANGE);
      tft.fillTriangle(x1, y1, x2, y2, x3, y3, TFT_ORANGE);
    }

    // Short scale tick length
    if (i % 64 != 0) tl = 8;

    // Recalculate coords incase tick lenght changed
    x0 = sx * (M_SIZE*100 + tl) + M_SIZE*120;
    y0 = sy * (M_SIZE*100 + tl) + M_SIZE*140;
    x1 = sx * M_SIZE*100 + M_SIZE*120;
    y1 = sy * M_SIZE*100 + M_SIZE*140;

    // Draw tick
    tft.drawLine(x0, y0, x1, y1, TFT_BLACK);

    // Check if labels should be drawn, with position tweaks
    if (i % 64 == 0) {
      // Calculate label positions
      x0 = sx * (M_SIZE*100 + tl + 10) + M_SIZE*120;
      y0 = sy * (M_SIZE*100 + tl + 10) + M_SIZE*140;
      switch (i / 64) {
        case -2: tft.drawCentreString("-128", x0, y0 - 12, 2); break;
        case -1: tft.drawCentreString("-64", x0, y0 - 9, 2); break;
        case 0: tft.drawCentreString("0", x0, y0 - 7, 2); break;
        case 1: tft.drawCentreString("64", x0, y0 - 9, 2); break;
        case 2: tft.drawCentreString("128", x0, y0 - 12, 2); break;
      }
    }

    // Now draw the arc of the scale
    sx = cos((i + 5 - 90) * 0.0174532925);
    sy = sin((i + 5 - 90) * 0.0174532925);
    x0 = sx * M_SIZE*100 + M_SIZE*120;
    y0 = sy * M_SIZE*100 + M_SIZE*140;
    // Draw scale arc, don't draw the last part
    if (i < 50) tft.drawLine(x0, y0, x1, y1, TFT_BLACK);
  }

  tft.drawString("%RH", M_SIZE*(5 + 230 - 40), M_SIZE*(119 - 20), 2); // Units at bottom right
  tft.drawCentreString("%RH", M_SIZE*120, M_SIZE*70, 4); // Comment out to avoid font 4
  tft.drawRect(5, 3, M_SIZE*230, M_SIZE*119, TFT_BLACK); // Draw bezel line

  plotNeedle(0, 0); // Put meter needle at 0
}

// #########################################################################
// Update needle position
// This function is blocking while needle moves, time depends on ms_delay
// 10ms minimises needle flicker if text is drawn within needle sweep area
// Smaller values OK if text not in sweep area, zero for instant movement but
// does not look realistic... (note: 100 increments for full scale deflection)
// #########################################################################
void plotNeedle(int value, byte ms_delay)
{
  // tft.fillScreen(TFT_DARKCYAN);
  tft.setTextColor(TFT_BLACK);
  char buf[8]; dtostrf(value, 4, 0, buf);
  tft.drawRightString(buf, M_SIZE*40, M_SIZE*(119 - 20), 2);

  if (value < -140) value = -140; // Limit value to emulate needle end stops
  if (value > 140) value = 140;

  // Move the needle until new value reached
  while (!(value == old_analog)) {
    if (old_analog < value) old_analog++;
    else old_analog--;

    if (ms_delay == 0) old_analog = value; // Update immediately if delay is 0

    float sdeg = map(old_analog, -140, 140, -150, -30); // Map value to angle
    // Calculate tip of needle coords
    float sx = cos(sdeg * 0.0174532925);
    float sy = sin(sdeg * 0.0174532925);

    // Calculate x delta of needle start (does not start at pivot point)
    float tx = tan((sdeg + 90) * 0.0174532925);

    // Erase old needle image
    tft.drawLine(M_SIZE*(120 + 20 * ltx - 1), M_SIZE*(140 - 20), osx - 1, osy, TFT_DARKCYAN);
    tft.drawLine(M_SIZE*(120 + 20 * ltx), M_SIZE*(140 - 20), osx, osy, TFT_DARKCYAN);
    tft.drawLine(M_SIZE*(120 + 20 * ltx + 1), M_SIZE*(140 - 20), osx + 1, osy, TFT_DARKCYAN);

    // Re-plot text under needle
    tft.setTextColor(TFT_BLACK);
    tft.drawCentreString("%RH", M_SIZE*120, M_SIZE*70, 4); // // Comment out to avoid font 4

    // Store new needle end coords for next erase
    ltx = tx;
    osx = M_SIZE*(sx * 98 + 120);
    osy = M_SIZE*(sy * 98 + 140);

    // Draw the needle in the new postion, magenta makes needle a bit bolder
    // draws 3 lines to thicken needle
    tft.drawLine(M_SIZE*(120 + 20 * ltx - 1), M_SIZE*(140 - 20), osx - 1, osy, TFT_RED);
    tft.drawLine(M_SIZE*(120 + 20 * ltx), M_SIZE*(140 - 20), osx, osy, TFT_MAGENTA);
    tft.drawLine(M_SIZE*(120 + 20 * ltx + 1), M_SIZE*(140 - 20), osx + 1, osy, TFT_RED);

    // Slow needle down slightly as it approaches new postion
    if (abs(old_analog - value) < 10) ms_delay += ms_delay / 2;

    // Wait before next update
    delay(ms_delay);
  }
}
