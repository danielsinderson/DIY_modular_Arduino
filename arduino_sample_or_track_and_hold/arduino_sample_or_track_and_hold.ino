/*
 * A sample-and-hold or track-and-hold module
 *  - Each module contains four S&Hs
 *  - Toggling between S&H and T&H is done with a switch
 *  - Each S&H requires four pins: one analog pin, one PWM pin, and two GPIOs. This caps the number of S&Hs per Arduino Nano at four (16/18 pins used) without complicating the peripheral circuitry.
 *  
 *  THIS MODULE IS FINISHED. No optimizations necessary.
 */

//PINS
const byte input_pins[4] = {A0, A2, A4, A6};
const byte output_pins[4] = {11, 10, 9, 3};
const byte trigger_pins[4] = {15, 17, 19, 21};
const byte mode_pins[4] = {5, 6, 7, 12};

bool toggles[4] = {0, 0, 0, 0};
bool triggers[4] = {0, 0, 0, 0};
byte output_values[4] = {0, 0, 0, 0};


void setup() {
  TCCR0B = TCCR0B & B11111000 | B00000010; // set PWM freq on pin 5/6 to ~7.8kHz
  TCCR1B = TCCR1B & B11111000 | B00000010; // set PWM freq on pin 9/10 to ~3.9kHz
  TCCR2B = TCCR2B & B11111000 | B00000010; // set PWM freq on pin 3/11 to ~3.9kHz
}

void loop() {
  for (byte i=0;i<4;i++) {
    //read pins
    bool mode = digitalRead(mode_pins[i]);
    trigger[i] = digitalRead(trigger_pins[i]);

    // S&H mode
    if (mode == 0) {
      if ((toggle[i] == 0) && (triggers[i] == 1)) {
        output_values[i] = analogRead(input_pins[i]) >> 2;
        toggle[i] = 1;
      }
      else if ((toggle[i] == 1) && (triggers[i] == 0)) {
        toggle[i] = 0;
      }
    }

    //T&H mode
    else {
      if (trigger[i] == 1) {
        output_values[i] = analogRead(input_pins[i]) >> 2;
      }
    }
    analogWrite(output_pins[i], output_values[i]);
  }

}
