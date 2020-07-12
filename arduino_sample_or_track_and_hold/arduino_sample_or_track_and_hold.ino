/*
 * A sample-and-hold or track-and-hold module
 *  - Each module contains four S&Hs
 *  - Toggling between S&H and T&H is done with a switch
 *  - Each S&H requires four pins: one analog pin, one PWM pin, and two GPIOs. This caps the number of S&Hs per Arduino Nano at four (16/18 pins used) without complicating the peripheral circuitry.
 *  
 *  THIS MODULE IS FINISHED. No optimizations necessary.
 */

//PINS
const byte input_pins[4] = {A0, A1, A2, A3};
const byte output_pins[4] = {6, 9, 10, 11};
const byte trigger_pins[4] = {2, 4, 8, 12};
const byte mode_pins[4] = {3, 5, 7, 18};

bool triggers[4] = {0, 0, 0, 0};
byte output_values[4] = {0, 0, 0, 0};


void setup() {
  
}

void loop() {
  for (byte i=0;i<4;i++) {
    //read pins
    bool mode = digitalRead(mode_pins[i]);
    bool toggle = digitalRead(trigger_pins[i]);

    // S&H mode
    if (mode == 0) {
      if ((toggle == 1) && (triggers[i] == 0)) {
        output_values[i] = analogRead(input_pins[i]) >> 2;
        triggers[i] = 1;
      }
      else if ((toggle == 0) && (triggers[i] == 1)) {
        triggers[i] = 0;
      }
      analogWrite(output_pins[i], output_values[i]);
    }

    //T&H mode
    else {
      if (toggle == 1) {
        output_values[i] = analogRead(input_pins[i]) >> 2;
      }
      analogWrite(output_pins[i], output_values[i]);
    }
  }

}
