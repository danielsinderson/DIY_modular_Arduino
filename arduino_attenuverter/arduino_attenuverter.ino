/*
 * An attenuverter module with DC offset. 
 *  - Each module contains two attenuverters w/ adjustable offset.
 *  - Gain and offset are controlled by potentiometers and whether the signal gets inverted is controlled by a switch
 *  - Each attenvuerter requires six pins: three analog pins, two PWM pin, and one GPIO. This caps the number of attenverters per Arduino Nano at two without compicating the peripheral circuitry.
 *  
 *  THIS MODULE IS FINISHED. No optimizations necessary.
 */

class Attenuverter
{
  //pins
  byte signalPin, factorPin, outputPin, invertPin, offsetPin, ledPin;

  //state variables
  int s, f, i, x, o, output; // signal, factor pin read, invert, intermediary variable, offset value, output

  public:
  Attenuverter(byte pin1, byte pin2, byte pin3, byte pin4, byte pin5, byte pin6)
  {
    signalPin = pin1;
    factorPin = pin2;
    outputPin = pin3;
    invertPin = pin4;
    offsetPin = pin5;
    ledPin = pin6;
  }

  void updateControlParams() {
    s = analogRead(signalPin) >> 2;
    f = analogRead(factorPin) >> 5;
    i = digitalRead(invertPin);
    o = analogRead(offsetPin) >> 2;

    if (i == 0) {
      output = (s * f) >> 5;
    }
    else {
      output = ((255 - s) * f) >> 5;
    }
    output += o;
    if (output > 255) { output = 255; }
  }

  void setupFunc() {
    pinMode(outputPin, OUTPUT);
    pinMode(ledPin, OUTPUT);
    pinMode(invertPin, INPUT);
  }

  void loopFunc() {
    analogWrite(outputPin, output);
    analogWrite(ledPin, output);
  }
};

Attenuverter a1(A0, A1, 10, 12, A2, 11); //signal, factor, output, invert, offset, led
Attenuverter a2(A3, A4, 5, 7, A5, 6);


unsigned long update_control_timer = 0;
int update_control_period = 30;



void setup() {
  TCCR0B = TCCR0B & B11111000 | B00000010; // set PWM freq on pin 5/6 to ~7.8kHz
  TCCR1B = TCCR1B & B11111000 | B00000010; // set PWM freq on pin 9/10 to ~3.9kHz
  TCCR2B = TCCR2B & B11111000 | B00000010; // set PWM freq on pin 3/11 to ~3.9kHz
}

void loop() {
  if (millis() - update_control_timer >= update_control_period) {
    a1.updateControlParams();
    a2.updateControlParams();
  }

  a1.loopFunc();
  a2.loopFunc();
}
