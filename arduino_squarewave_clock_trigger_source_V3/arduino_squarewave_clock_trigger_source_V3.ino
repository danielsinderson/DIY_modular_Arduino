/*
 * A square wave module with audio and LFO frequency modes. Usable as a clock/gate/trigger source or a square wave tone. 
 *  - Each module contains four square wave oscillators.
 *  - Frequency is controlled by a potentiometer and a switch. Range is ~0.25Hz to 10Hz in LFO mode and ~30Hz to ~500Hz in audio mode
 *  - Each oscillator requires three pins: one analog pin and two GPIO.
 *  
 *  THIS MODULE IS FINISHED. No optimizations necessary.
 *  
 *  V2: Removed two oscillators and added control over duty cycle to each.
 *  V3: Changed the way the frequenies are calculated, using an array of preset values instead of a rough calculation. This is to give the spread of frequencies a logarithmic feel, making selecting one's desired frequency much easier.
 */


 float note_freq[25] = {55.00, 58.27, 61.74, 65.41, 69.30, 73.42, 77.78, 82.41, 87.31, 92.50, 98.00, 103.83, 
                       110.00, 116.54, 123.47, 130.81, 138.59, 146.83, 155.56, 164.81, 174.61, 185.00, 196.00, 207.65, 
                       220.00};

float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
 return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

class SquareWaveClock
{
  //pins
  byte periodPin, modePin, outputPin, dutyCyclePin, ledPin;
  
  //state variables
  unsigned long previousMillis;
  unsigned period, low, high, dutyCycle;
  bool mode, output;

  float freq;

  public:
  SquareWaveClock(byte pin1, byte pin2, byte pin3, byte pin4, byte pin5)
  {
    periodPin = pin1;
    modePin = pin2;
    outputPin = pin3;
    dutyCyclePin = pin4;
    ledPin = pin5;
  }

  void toggleOutput() {
    if (output==0) { output = 1; }
    else { output = 0; }
  }

  void updateControlParams() {
    mode = digitalRead(modePin);
    if (mode == 0) { 
      period = period = 34 - (analogRead(periodPin) >> 5);;
    }
    else {
      freq = pow(2, mapFloat(analogRead(periodPin), 0, 1023, -1, 3));
      period = round((1000 * (1/freq)));
    }

    high = map(analogRead(dutyCyclePin), 0, 1023, 0, period);
    low = period - high; 
  }

  void setupFunc() {
    pinMode(outputPin, OUTPUT);
    pinMode(ledPin, OUTPUT);
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
    digitalWrite(ledPin, output);
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



SquareWaveClock swc1(A0, 2, 3, A1, 4);
SquareWaveClock swc2(A2, 5, 6, A3, 7);
SquareWaveClock swc3(A4, 8, 9, A5, 10);
SquareWaveClock swc4(A6, 11, 12, A7, 13);


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
