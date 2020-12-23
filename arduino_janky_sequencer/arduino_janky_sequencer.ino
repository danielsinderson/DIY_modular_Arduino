/*
 * A janky sort of sequencer module. It takes a trigger and an analog signal. 
 * When the trigger goes high it samples a 6-bit value from the analog signal and uses those bits to toggle its outputs ON/OFF. Outputs are spit out on PORTB.
 * It also has the option to read a second analog signal and use some bitwise operators to manipulate to outputs.
 * 
 * MODULE FINISHED. No optimizations necessary.
 */



//pins
const byte inputPins[2] = {A0, A1};
const byte modePin = A2;
const byte triggerPin = 17;

//state variables
bool trigger = 0;
byte output = 0;
byte in1 = 0;
byte in2 = 0;
byte mode = 0;


void read_trigger() {
  if (digitalRead(triggerPin) == true and trigger == false) {
    in1 = analogRead(inputPins[0]) >> 4;
    in2 = analogRead(inputPins[1]) >> 4;
    trigger = true;
  }
  else if (digitalRead(triggerPin) == false and trigger == true) {
    trigger = false;
  }
}


void setup() {
  for (byte i=8;i<14;i++) {
    pinMode(i, OUTPUT);
  }
}

void loop() {
  read_trigger();

  mode = analogRead(modePin) >> 8;
  if (mode == 0) { output = in1; }
  else if (mode == 1) { output = in1 | in2; }
  else if (mode == 2) { output = in1 & in2; }
  else { output = in1 ^ in2; }

  PORTB = output;
}
