/*
 * A quantizer module.
 *  - Each module contains two quantizers
 *  - Allowed notes are determined using switches to toglle each tone and potentiometers to set the octave range
 *  - Each quantizer requires four pins: three analog pins and one PWM pin. Plus they share the tone toggles which require twelve pins; this uses every pin on the Arduino Nano. 
 *  
 *  THIS MODULE IS FINISHED. No optimizations necessary.
 */


 //pins
 byte tonePins[12] = {2, 3, 4, 5, 6, 7, 8, 11, 12, 13, 14, 15};

//note to output value table
byte toneArray[60];


class Quantizer
{
  //pins
  byte inputPin, outputPin, rangeStartPin, rangeEndPin;

  byte startOctave, endOctave, numActiveNotes, numTotalNotes, output, index;
  bool notes[12];
  byte quantizerArray[60];

  public:
  Quantizer(byte pin1, byte pin2, byte pin3, byte pin4)
  {
    inputPin = pin1;
    outputPin = pin2;
    rangeStartPin = pin3;
    rangeEndPin = pin4;
  }

  void updateControlParams() {
    //update all basic parameters
    startOctave = map(analogRead(rangeStartPin), 0, 1023, 0, 4);
    endOctave = min(5, startOctave + map(analogRead(rangeEndPin), 0, 1023, 1, 5));
    for (byte i=0;i<12;i++) {
      notes[i] = digitalRead(tonePins[i]);
    }
    
    numActiveNotes = 0;
    for (byte i=0;i<12;i++) {
      numActiveNotes += notes[i];
    }
    numTotalNotes = numActiveNotes * (endOctave - startOctave);

    //update quantizer tone array
    index = 0;
    for (byte i=startOctave*12; i<endOctave*12; i++) {
      byte x = i%12;
      if (notes[x] == true) {
        quantizerArray[index] = toneArray[i];
        index++;
      }
    }
  }

  void loopFunc() {
    byte y = map(analogRead(inputPin), 0, 1023, 0, numTotalNotes-1);
    output = quantizerArray[y];
    analogWrite(outputPin, output);
  }
  
};

Quantizer q1(A2, 9, A3, A4); //input, output, rangeStart, rangeEnd
Quantizer q2(A5, 10, A6, A7);

unsigned long update_control_timer = 0;
int update_control_period = 30;



void setup() {
  update_control_timer = millis();
  for (byte i=0;i<60;i++) {
    toneArray[i] = (byte) ceil(4.25 * i);
  }
  TCCR0B = TCCR0B & B11111000 | B00000010; // set PWM freq on pin 5/6 to ~7.8kHz
  TCCR1B = TCCR1B & B11111000 | B00000010; // set PWM freq on pin 9/10 to ~3.9kHz
  TCCR2B = TCCR2B & B11111000 | B00000010; // set PWM freq on pin 3/11 to ~3.9kHz
}

void loop() {
  if (millis() - update_control_timer >= update_control_period) {
    q1.updateControlParams();
    q2.updateControlParams();
  }

  q1.loopFunc();
  q2.loopFunc();
}
