const int sampleWindow = 50; // Sample window width in mS (50 mS = 20Hz)
const int A1A = 3;
const int A1B = 5;
unsigned int sample;

void setup() {
Serial.begin(9600);
pinMode(A1A, OUTPUT);
pinMode(A1B, OUTPUT);
digitalWrite(A1A, LOW);
digitalWrite(A1B, LOW);
}

void loop() {
unsigned long startMillis= millis();  // Start of sample window
unsigned int peakToPeak = 0;   // peak-to-peak level
unsigned int signalMax = 0;
unsigned int signalMin = 1024; 

// collect data for 50 mS
while (millis() - startMillis < sampleWindow) {
sample = analogRead(A0);   //reading DC pin from pin A0
if (sample < 1024)  // toss out spurious readings {
if (sample > signalMax) { 
signalMax = sample;  // save just the max levels 
}
else if (sample < signalMin) {
signalMin = sample;  // save just the min levels
}
}
peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
Serial.println(peakToPeak);


if (peakToPeak<=75) { // adjust this value to make more and less sensitive!
digitalWrite(A1A,LOW); // turn motor off
digitalWrite(A1B,LOW);
}

else if (peakToPeak > 75) { // adjust this value to make more and less sensitive!
digitalWrite(A1A,50); // turn motor on
digitalWrite(A1B,LOW);
delay(2000);
}
}