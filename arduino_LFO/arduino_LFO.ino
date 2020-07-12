/*
 * A LFO module using 64-sample wavetable for eight different waveforms.
 *  - Each module contains four LFOs with independent waveform and frequency control
 *  - Frequencies range from ~0.125Hz to ~15Hz
 *  - Each LFO requires three pins: two analog pins and one PWM pin. This caps the number of LFO per Arduino Nano at four without compicating the peripheral circuitry. 
 *  
 *  THIS MODULE IS FINISHED. No optimizations necessary.
 */


byte wavetableLength = 64;

/*
 * sine
 * triangle
 * halfsine
 * square
 * saw
 * random pulse
 * random saw
 * random noise
 */
byte wavetables[8][64] = {{127, 139, 151, 163, 175, 186, 197, 207, 216, 225, 232, 239, 244, 248, 251, 253, 254, 253, 251, 248, 244, 239, 232, 225, 216, 207, 197, 186, 175, 163, 151, 139, 127, 114, 102, 90, 78, 67, 56, 46, 37, 28, 21, 14, 9, 5, 2, 0, 0, 0, 2, 5, 9, 14, 21, 28, 37, 46, 56, 67, 78, 90, 102, 114},
                          {0, 8, 16, 24, 32, 40, 48, 56, 64, 72, 80, 88, 96, 104, 112, 120, 128, 136, 144, 152, 160, 168, 176, 184, 192, 200, 208, 216, 224, 232, 240, 248, 248, 240, 232, 224, 216, 208, 200, 192, 184, 176, 168, 160, 152, 144, 136, 128, 120, 112, 104, 96, 88, 80, 72, 64, 56, 48, 40, 32, 24, 16, 8, 0},
                          {0, 12, 24, 37, 49, 61, 74, 85, 97, 109, 120, 131, 141, 151, 161, 171, 180, 188, 197, 204, 212, 218, 224, 230, 235, 240, 244, 247, 250, 252, 253, 254, 255, 254, 253, 252, 250, 247, 244, 240, 235, 230, 224, 218, 212, 204, 197, 188, 180, 171, 161, 151, 141, 131, 120, 109, 97, 85, 74, 61, 49, 37, 24, 12},
                          {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                          {0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64, 68, 72, 76, 80, 84, 88, 92, 96, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140, 144, 148, 152, 156, 160, 164, 168, 172, 176, 180, 184, 188, 192, 196, 200, 204, 208, 212, 216, 220, 224, 228, 232, 236, 240, 244, 248, 252},
                          {0, 0, 0, 255, 255, 0, 255, 255, 0, 0, 0, 255, 0, 0, 0, 0, 255, 0, 0, 255, 255, 255, 0, 255, 0, 255, 0, 255, 255, 0, 0, 0, 0, 0, 0, 0, 255, 255, 0, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 0, 0, 255, 255, 0, 0, 255},
                          {0, 0, 8, 0, 0, 0, 0, 28, 0, 36, 0, 44, 0, 52, 56, 0, 0, 68, 72, 76, 80, 0, 88, 0, 96, 100, 0, 108, 112, 116, 120, 0, 128, 132, 0, 140, 144, 0, 0, 0, 160, 164, 168, 0, 176, 0, 0, 188, 0, 196, 0, 0, 208, 212, 0, 0, 0, 0, 232, 236, 240, 0, 0, 0},
                          {243, 109, 118, 169, 49, 242, 235, 54, 78, 102, 10, 108, 166, 238, 27, 0, 172, 215, 227, 134, 187, 77, 104, 230, 218, 44, 246, 101, 216, 19, 187, 99, 248, 87, 52, 160, 165, 156, 188, 36, 49, 225, 75, 226, 24, 182, 71, 177, 155, 117, 207, 207, 133, 165, 47, 181, 250, 102, 22, 214, 146, 62, 131, 86}};

class LFO
{
  //pins
  byte sampleRatePin, waveformPin, outputPin;

  //state variables
  unsigned long previousMillis;
  unsigned int sampleRate;
  byte waveform, output, sampleIndex;

  public:
  LFO(byte pin1, byte pin2, byte pin3)
  {
    sampleRatePin = pin1;
    waveformPin = pin2;
    outputPin = pin3;
  }

  void updateControlParams() {
    sampleRate = 128 - (analogRead(sampleRatePin) >> 3); 
    waveform = analogRead(waveformPin) >> 7;
  }

  void setupFunc() {
    pinMode(outputPin, OUTPUT);
    previousMillis = millis();
    output = 0;
  }

  void loopFunc() {
    if (millis() - previousMillis >= sampleRate) {
      sampleIndex++;
      previousMillis = millis();
      if (sampleIndex >= wavetableLength) {
        sampleIndex = 0;
      }
    }
    output = wavetables[waveform][sampleIndex];
    analogWrite(outputPin, output);
    //Serial.println(sampleIndex);
  }
};


LFO lfo1(A0, A1, 11);
LFO lfo2(A2, A3, 10);
LFO lfo3(A4, A5, 9);
LFO lfo4(A6, A7, 6);

unsigned long update_control_timer = 0;
int update_control_period = 30;




void setup() {
  lfo1.setupFunc();
  lfo2.setupFunc();
  lfo3.setupFunc();
  lfo4.setupFunc();
  update_control_timer = millis();
  //Serial.begin(9600);
}

void loop() {
  lfo1.loopFunc();
  lfo2.loopFunc();
  lfo3.loopFunc();
  lfo4.loopFunc();

  if (millis() - update_control_timer >= update_control_period) {
    lfo1.updateControlParams();
    lfo2.updateControlParams();
    lfo3.updateControlParams();
    lfo4.updateControlParams();
  }
}
