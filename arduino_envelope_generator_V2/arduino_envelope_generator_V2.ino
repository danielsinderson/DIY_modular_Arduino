 /*
 * 128 sample AD envelope module with optional sustain and linear, exponential, and logarithmic curves
 *  - Each module contains two AD envelope generators.
 *  - Attack time, decay time, and wave shape are controlled with potentiometers while sustain is controlled by a switch. 
 *  - Each AD envelope generator requires six pins: three analog pins, one PWM pin, and two GPIO. This caps the number of AD envelope generators per Arduino Nano at two without compicating the peripheral circuitry.
 *  
 *  THIS MODULE IS FINISHED. No optimizations necessary.
 */


//wavetables
const byte linear_curve[64] = {0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64, 
                                68, 72, 76, 80, 84, 88, 92, 96, 100, 104, 108, 112, 116, 120, 124, 128, 
                                132, 136, 140, 144, 148, 152, 156, 160, 164, 168, 172, 176, 180, 184, 188, 192, 
                                196, 200, 204, 208, 212, 216, 220, 224, 228, 232, 236, 240, 244, 248, 252};

const byte log_curve[64] = {0, 16, 30, 43, 54, 64, 73, 82, 90, 97, 104, 111, 117, 123, 128, 134, 139, 144, 148, 
                             153, 157, 161, 165, 169, 173, 176, 180, 183, 186, 189, 193, 196, 198, 201, 204, 207, 
                             209, 212, 215, 217, 219, 222, 224, 226, 229, 231, 233, 235, 237, 239, 241, 243, 245, 
                             247, 249, 250, 252, 254, 255, 255, 255, 255, 255, 255};

const byte exp_curve[64] = {0, 0, 0, 0, 0, 0, 1, 3, 5, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 29, 31, 33, 
                             36, 38, 40, 43, 46, 48, 51, 54, 57, 59, 62, 66, 69, 72, 75, 79, 82, 86, 90, 94, 
                             98, 102, 107, 111, 116, 121, 127, 132, 138, 144, 151, 158, 165, 173, 182, 191, 
                             201, 212, 225, 239, 255};


//Functions
int time_map(int input) { //2ms per sample to 20ms per sample
  return (input >> 4) + 1;
}
int waveshape_map(int input) {
  return (input*9) >> 10;
}


class EnvelopeGenerator
{
  //pins
  byte attackPin, decayPin, shapePin, triggerPin, sustainPin, outputPin;

  //state variables
  bool trigger = 0;
  bool sustain = 0;
  bool note_on = 0;
  byte curve_shape = 0;

  byte attack_index = 0;
  byte decay_index = 0;

  unsigned long last_time_of_attack_sample;
  unsigned long last_time_of_decay_sample;
  int attack_sample_rate = 4;
  int decay_sample_rate = 4;

  public:
  EnvelopeGenerator(byte pin1, byte pin2, byte pin3, byte pin4, byte pin5, byte pin6)
  {
    attackPin = pin1;
    decayPin = pin2;
    shapePin = pin3;
    triggerPin = pin4;
    sustainPin = pin5;
    outputPin = pin6;
  }

  void update_controls() {
    attack_sample_rate = time_map(analogRead(attackPin));
    decay_sample_rate = time_map(analogRead(decayPin));
    curve_shape = waveshape_map(analogRead(shapePin));
    sustain = digitalRead(sustainPin);
  }

  byte attack_choice(byte n) {
    if (n < 3) {return linear_curve[attack_index];}
    else if (n < 6) {return exp_curve[attack_index];}
    else {return log_curve[attack_index];}
  }

  byte decay_choice(byte n) {
    if ((n == 0) or (n==3) or (n==6)) {return linear_curve[63 - decay_index];}
    else if ((n==1) or (n==4) or (n==7)) {return exp_curve[63 - decay_index];}
    else {return log_curve[63 - decay_index];}
  }

  void read_trigger() {
    if (digitalRead(triggerPin) == true and trigger == false) {
      trigger = true;
      attack_index = 0;
      note_on = true;
    }
    else if (digitalRead(triggerPin) == false and trigger == true) {
      trigger = false;
      decay_index = 0;
      note_on = false;
    }
  }

  void loopFunc() {
    //read trigger pin and set boolean variables depending on state
    read_trigger();
  
    if (note_on == true) {
      analogWrite(outputPin, attack_choice(curve_shape));
      if ((sustain == false) and (attack_index >= 63)) { 
        note_on = false; 
      }
      else if ((millis() - last_time_of_attack_sample >= attack_sample_rate) and (attack_index < 63)) { 
        attack_index++;
        last_time_of_attack_sample = millis();
      }
    }
  
    else {
      analogWrite(outputPin, decay_choice(curve_shape));
      if ((millis() - last_time_of_decay_sample >= decay_sample_rate) and (decay_index < 63)) {
        decay_index++;
        last_time_of_decay_sample = millis();
      }
    }
  }
};

EnvelopeGenerator e1(A0, A1, A2, 12, 11, 10);
EnvelopeGenerator e2(A3, A4, A5, 8, 7, 6);


unsigned long last_time_of_update;
int update_controls_delta = 30; //every 512ms update control parameters


void setup() {
  TCCR0B = TCCR0B & B11111000 | B00000010; // set PWM freq on pin 5/6 to ~7.8kHz
  TCCR1B = TCCR1B & B11111000 | B00000010; // set PWM freq on pin 9/10 to ~3.9kHz
  TCCR2B = TCCR2B & B11111000 | B00000010; // set PWM freq on pin 3/11 to ~3.9kHz
}



void loop() {
  //update control parameters periodically based on update_controls_delta variable
  if (millis() - last_time_of_update >= update_controls_delta) { 
    e1.update_controls();
    e2.update_controls();
  }

  e1.loopFunc();
  e2.loopFunc();
}
