/*
 * A square wave module with audio and LFO frequency modes. Usable as a clock/gate/trigger source or a square wave tone. 
 *  - Each module contains four square wave oscillators.
 *  - Frequency is controlled by a potentiometer and a switch. Range is ~0.25Hz to 10Hz in LFO mode and ~30Hz to ~500Hz in audio mode
 *  - Each oscillator requires three pins: one analog pin and two GPIO.
 *  
 *  THIS MODULE IS FINISHED. No optimizations necessary.
 *  
 *  EDIT! V2 WILL SACRIFICE TWO OSCILLATORS FOR PULSE WIDTH CONTROL! I think this will be super nice to have for the LFO and also a pretty fun option for the VCO.
 */


class SquareWaveClock
{
  //pins
  byte periodPin, modePin, outputPin, dutyCyclePin;
  
  //state variables
  unsigned long previousMillis;
  int period, low, high, dutyCycle;
  bool mode, output;

  public:
  SquareWaveClock(byte pin1, byte pin2, byte pin3, byte pin4)
  {
    periodPin = pin1;
    modePin = pin2;
    outputPin = pin3;
    dutyCyclePin = pin4;
  }

  void toggleOutput() {
    if (output==0) { output = 1; }
    else { output = 0; }
  }

  void updateControlParams() {
    mode = digitalRead(modePin);
    if (mode == 0) { period = 4196 - (analogRead(periodPin) << 2); }
    else           { period = 34 - (analogRead(periodPin) >> 5); }

    high = map(analogRead(dutyCyclePin), 0, 1023, 0, period);
    low = period - high; 
  }

  void setupFunc() {
    pinMode(outputPin, OUTPUT);
    pinMode(modePin, INPUT);
    previousMillis = millis();
    output = LOW;
  }

  void loopFunc() {
    if (output == LOW) {
      if (millis() - previousMillis >= low) { 
        toggleOutput(); 
        previousMillis = millis();
      }
    }
    else {
      if (millis() - previousMillis >= high) { 
        toggleOutput(); 
        previousMillis = millis();
      }
    }
    
    digitalWrite(outputPin, output);
  }

/*
  void debug() {
    Serial.print("MODE: ");
    Serial.print(mode);
    Serial.print("\n");
    Serial.print("PERIOD");
    Serial.print(period);
    Serial.print("\n");
    Serial.print("PREVIOUS MILLIS VALUE: ");
    Serial.print(previousMillis);
    Serial.print("\n");
    Serial.print("\n");
    Serial.print("\n");
  }
  */
};



SquareWaveClock swc1(A0, 2, 3, A4);
SquareWaveClock swc2(A1, 4, 5, A5);
SquareWaveClock swc3(A2, 6, 7, A6);
SquareWaveClock swc4(A3, 8, 9, A7);


unsigned long update_control_timer = 0;
int update_control_period = 30;

//int debug_counter = 0;

void setup() {
  swc1.setupFunc();
  swc2.setupFunc();
  swc3.setupFunc();
  swc4.setupFunc();
  update_control_timer = millis();
  //Serial.begin(9600);
}

void loop() {
  swc1.loopFunc();
  swc2.loopFunc();
  swc3.loopFunc();
  swc4.loopFunc();

  if (millis() - update_control_timer >= update_control_period) {
    swc1.updateControlParams();
    swc2.updateControlParams();
    swc3.updateControlParams();
    swc4.updateControlParams();
  }
}
