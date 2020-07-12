/*
 * 
 */


#include <AceButton.h>
#include <AdjustableButtonConfig.h>
#include <ButtonConfig.h>
using namespace ace_button;

#include <Adafruit_NeoPixel.h>
#include <Encoder.h>

// Neopixel variables
const byte neoPixelPin = 9;
const byte neoPixelCount = 9;
Adafruit_NeoPixel leds(neoPixelCount, neoPixelPin, NEO_RGB + NEO_KHZ800);

// Rotary encoder variables
Encoder encoder(2, 3);
const byte L = neoPixelCount - 1; //max sequence length
byte counters[3][4] = {{4, 0, 0, 4},
                       {4, 0, 0, 4},
                       {4, 0, 0, 4}};
byte values[3][4] = {{1, 0, 0, 1},   // {seq length, beats, offset, clk divider}
                     {1, 0, 0, 1},
                     {1, 0, 0, 1}};
byte maxes[3][4] = {{L, L, L, L},
                    {L, L, L, L},
                    {L, L, L, L}};
byte mins[3][4] = {{1, 0, 0, 1},
                   {1, 0, 0, 1},
                   {1, 0, 0, 1}};

//button variables
const byte buttonPin = 4;
AceButton button(buttonPin);
unsigned long seqSelectCounter = 0;
byte seqSelect = 0;
unsigned long paramSelectCounter = 0;
byte paramSelect = 0;
void handleEvent(AceButton*, uint8_t, uint8_t);

//Euclidean Sequencer variables
bool outputArrays[3][L] = {{0, 0, 0, 0, 0, 0, 0, 0},
                           {0, 0, 0, 0, 0, 0, 0, 0},
                           {0, 0, 0, 0, 0, 0, 0, 0}};
byte outputIndexes[3] = {0, 0, 0};
unsigned long outputClockCounters[3] = {0, 0, 0};

//trigger variables
const byte outputPins[3] = {6, 7, 8};
const byte triggerPin = 12;
bool trigger = 0;
byte triggerCounts[3] = {0, 0, 0};
unsigned long indexCounters[3] = {0, 0, 0};
byte indexes[3] = {0, 0, 0};

byte modePin = 10;
bool mode = 0;
unsigned long beatCounter = 0;
byte beatSelect = 0;
bool outputArrays2[3][L] = {{0, 0, 0, 0, 0, 0, 0, 0},
                           {0, 0, 0, 0, 0, 0, 0, 0},
                           {0, 0, 0, 0, 0, 0, 0, 0}};
byte indexes2[3] = {0, 0, 0};


unsigned long update_control_timer = 0;
int update_control_period = 100;



void setup() {
  for (byte i=0;i<3;i++) {
    pinMode(outputPins[i], OUTPUT);
  }
  
  leds.begin();
  leds.show();

  pinMode(buttonPin, INPUT_PULLUP);
  button.init(buttonPin, HIGH);
  ButtonConfig* buttonConfig = button.getButtonConfig();
  buttonConfig->setEventHandler(handleEvent);
  buttonConfig->setFeature(ButtonConfig::kFeatureDoubleClick);
  buttonConfig->setFeature(
      ButtonConfig::kFeatureSuppressClickBeforeDoubleClick);
  buttonConfig->setFeature(ButtonConfig::kFeatureSuppressAfterClick);
  buttonConfig->setFeature(ButtonConfig::kFeatureSuppressAfterDoubleClick);

  Serial.begin(115200);
}


void loop() {

  //mode read block
  bool newRead = digitalRead(modePin);
  if (newRead == HIGH and mode == LOW) {
    encoder.write(0);
  }
  else if (newRead == LOW and mode == HIGH) {
    encoder.write(counters[seqSelect][paramSelect]);
  }
  mode = newRead;

  //Button block
  button.check();

  //Rotary encoder block
  long newPosition = encoder.read();

  if (mode == 0) {
    byte x = maxes[seqSelect][paramSelect] << 2;
    byte y = mins[seqSelect][paramSelect] << 2;
    if (newPosition > x) { 
      newPosition = x;
      encoder.write(x);
    }
    else if (newPosition < y) {
      newPosition = y;
      encoder.write(y);
    }
  
    if (newPosition != counters[seqSelect][paramSelect]) {
      counters[seqSelect][paramSelect] = newPosition;
      //printUpdate();
      //printSeqs();
    }
  }
  else {
    if (newPosition != beatCounter) {
      beatCounter = newPosition;
      beatSelect = (beatCounter >> 2) % L;
      //printUpdate();
      //printSeqs();
    }
  }
  
  
  updateParamValues();
  updateParamMaxes();

  for (byte i=0; i<3; i++) {
    if (values[i][1] == 0) { clearOutputArray(i); }
    else { setOutputArray(i); }
  }


  //trigger block
  bool t = digitalRead(triggerPin);
  if (t == HIGH and trigger == LOW) {
    trigger = HIGH;
    if (mode == 0) {
      for (byte i=0;i<3;i++) { 
        triggerCounts[i]++;
        if (triggerCounts[i] >= values[i][3]) { // if trigger counter for seq is equal to the clk divider inc index and reset to zero
          triggerCounts[i] = 0;
          indexCounters[i]++;
        }
      }      
    }
    else {
      for (byte i=0;i<3;i++) { 
        indexes2[i]++;
        if (indexes2[i] >= L) { indexes2[i] = 0; }
      }
    }
  }
  
  else if (t == LOW and trigger == HIGH) {
    trigger = LOW;
  }
  for (byte i=0; i<3; i++) {
    indexes[i] = (indexCounters[i] + values[i][2]) % values[i][0]; 
  }


  //NeoPixel block
  if (millis() - update_control_timer >= update_control_period) {
    setLEDS();
    leds.show(); 
  }

  //output block
  if (mode == 0) {
    for (byte i=0;i<3;i++) {
      if (outputArrays[i][indexes[i]] == 1) {
        digitalWrite(outputPins[i], HIGH);
      }
      else {
        digitalWrite(outputPins[i], LOW);
      }
    }
  }
  else {
    for (byte i=0;i<3;i++) {
      if (outputArrays2[i][indexes2[i]] == 1) {
        digitalWrite(outputPins[i], HIGH);
      }
      else {
        digitalWrite(outputPins[i], LOW);
      }
    }
  }

}



////////////////////////////////////////////////////////
/*
 * FUNCTIONS 
 */

void toggleBeat() {
  if (outputArrays2[seqSelect][beatSelect] == 0) {
    outputArrays2[seqSelect][beatSelect] = 1;
  }
  else {
    outputArrays2[seqSelect][beatSelect] = 0;
  }
}

// The event handler for the button.
void handleEvent(AceButton* /* button */, uint8_t eventType,
    uint8_t buttonState) {
  switch (eventType) {
    case AceButton::kEventClicked:
      break;
    case AceButton::kEventReleased:
      seqSelectCounter++;
      seqSelect = seqSelectCounter % 3;
      if (mode == 0) {
        paramSelectCounter = 0;
        paramSelect = paramSelectCounter % 4;
        encoder.write(counters[seqSelect][paramSelect]);
      }
      break;
    case AceButton::kEventDoubleClicked:
      if (mode == 0) {
        paramSelectCounter++;
        paramSelect = paramSelectCounter % 4;
        encoder.write(counters[seqSelect][paramSelect]);
      }
      else {
        toggleBeat();
      }
      break;
    case AceButton::kEventLongPressed:
      break;
  }
}

void setLEDS() {
  // sequencer LEDs
  if (mode == 0) {
    for (byte i=0; i<L; i++) {
      if (i == indexes[seqSelect]) { leds.setPixelColor(i, 60, 60, 60); }
      else if (outputArrays[seqSelect][i] == 1) {
        if (seqSelect == 0) { leds.setPixelColor(i, 150, 0, 0); }
        else if (seqSelect == 1) { leds.setPixelColor(i, 0, 150, 0); }
        else { leds.setPixelColor(i, 0, 0, 150); }
      }
      else if (i < values[seqSelect][0]) { 
        if (seqSelect == 0) { leds.setPixelColor(i, 15, 3, 3); }
        else if (seqSelect == 1) { leds.setPixelColor(i, 3, 15, 3); }
        else { leds.setPixelColor(i, 3, 3, 15); }
      }
      else { leds.setPixelColor(i, 0, 0, 0); }
    }

    // Param LED
    if (paramSelect == 0) { leds.setPixelColor(L, 100, 0, 50); }
    else if (paramSelect == 1) { leds.setPixelColor(L, 50, 100, 0); }
    else if (paramSelect == 2) { leds.setPixelColor(L, 0, 75, 75); }
    else { leds.setPixelColor(L, 60, 60, 60); }
  }

  else {
    leds.setPixelColor(L, 0, 0, 0);
    for (byte i=0; i<L; i++) {
      if (i == indexes2[seqSelect]) { leds.setPixelColor(i, 60, 60, 60); }
      else if (outputArrays2[seqSelect][i] == 1) {
        if (seqSelect == 0) { leds.setPixelColor(i, 125, 0, 75); }
        else if (seqSelect == 1) { leds.setPixelColor(i, 75, 125, 0); }
        else { leds.setPixelColor(i, 0, 75, 125); }
      }
      else { leds.setPixelColor(i, 0, 0, 0); }
    }
    leds.setPixelColor(beatSelect, 255, 100, 25);
  }
}

void updateParamMaxes() {
  for (byte row=0; row<3; row++) {
    for (byte col=1; col<3; col++) {
      maxes[row][col] = values[row][0];
    }
  }
}

void updateParamValues() {
  for (byte row=0; row<3; row++) {
    for (byte col=0; col<4; col++) {
      values[row][col] = counters[row][col] / 4;
    }
  }
}

void printUpdate() {
  for (byte i=0; i<3; i++) {
    Serial.print("Sequencer ");
    Serial.print(i+1);
    Serial.print(" Params: ");
    Serial.print(values[i][0]);
    Serial.print(", ");
    Serial.print(values[i][1]);
    Serial.print(", ");
    Serial.print(values[i][2]);
    Serial.print(", ");
    Serial.print(values[i][3]);
    Serial.print(", "); 
    Serial.print("\n");
  }
  Serial.print("\n\n\n");
}

void printSeqs() {
  for (byte i=0; i<3; i++) {
    Serial.print("Sequencer ");
    Serial.print(i);
    Serial.print(": ");
    for (byte j=0; j<L; j++) {
      Serial.print(outputArrays[i][j]);
    }
    Serial.print("\n");
  }
  Serial.print("\n\n\n");
}

void clearOutputArray(byte seq) {
  for (byte i=0; i<L; i++) {
    outputArrays[seq][i] = 0;
  }
}


void setOutputArray(byte seq) { // {seq length, beats, offset, clk divider}
      // find all variable values and populate g array
      byte n = values[seq][0];
      byte k = values[seq][1];

      byte d = n / k;
      byte r = n % k;
      
      byte a_size = d;
      byte a_count = k - r;
      byte b_size = d + 1;
      byte b_count = r;
      byte g[4];
      if (a_count > b_count) {
        g[0] = a_size;
        g[1] = a_count;
        g[2] = b_size;
        g[3] = b_count;
      }
      else {
        g[0] = b_size;
        g[1] = b_count;
        g[2] = a_size;
        g[3] = a_count;
      }

      //clear array
      for (byte i=0; i<L; i++) {
        outputArrays[seq][i] = 0;
      }
      
      //do algorithm
      byte counter = 1;
      byte c_mod;
      byte index = 0;
      if (g[3] != 0) { c_mod = (g[1] / g[3]) + 1; }
      else { c_mod = 2; }

      while (g[1] > 0 or g[3] > 0) {
        outputArrays[seq][index] = 1;
        if (counter % c_mod == 0) {
          if (g[3] > 0) {
            index += g[2];
            g[3] -= 1;
          }
        }
        else {
          if (g[1] > 0) {
            index += g[0];
            g[1] -= 1;          
          }
        }
        counter++;
      }
    }




    
