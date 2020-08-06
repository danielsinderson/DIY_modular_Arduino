/*
 * A VCO+VCA module.
 *  - Each module contains a 14-bit VCO with six different waveforms and a VCA
 *  - Requires four pins: two analog pins and two PWM pins for Mozzi HI-FI output mode. 
 *  
 *  THIS MODULE IS FINISHED (for now). 
 *   - No optimizations are necessary, but I might wan to add more waveform types or even add a second VCO+VCA and just sum the outputs. 
 *     This would lower the resolution to 13-bit and wonkify the volume some but it would be really easy to code.
 */


#include <MozziGuts.h>
#include <Oscil.h>
#include <AutoMap.h>
#include <ADSR.h>
#include <LowPassFilter.h>

#include <tables/sin2048_int8.h>
#include <tables/halfsin2048_int8.h>
#include <tables/square_no_alias_2048_int8.h>
#include <tables/saw2048_int8.h>
#include <tables/triangle2048_int8.h>
#include <tables/homebrew_clash2048_int8.h>


//PINS!
const byte analog_inputs[5] = {A0, A1, A2, A3, A4};  // {tone, volume, waveform, cutoff, resonance}
const byte trigger_pin = 4;


//global variables
bool trigger = LOW;
byte tone_index = 0;
byte volume = 0;
byte waveform = 0;
byte cutoff = 20;
byte resonance = 0;

//oscillator
Oscil <2048, AUDIO_RATE> oscillator;

//envelope
//ADSR <AUDIO_RATE, AUDIO_RATE> envelope;

//low pass filter
//LowPassFilter lpf; // cutoff 0-255 -> 0-8192Hz; resonance 0-255 -> low-high

//maps
AutoMap tone_index_map(0, 1023, 0, 59);
AutoMap wavetable_choice_map(0, 1023, 0, 5);


//Quantized frequencies array from A1 to A7
float note_freq[60] = {55.00, 58.27, 61.74, 65.41, 69.30, 73.42, 77.78, 82.41, 87.31, 92.50, 98.00, 103.83, 
                       110.00, 116.54, 123.47, 130.81, 138.59, 146.83, 155.56, 164.81, 174.61, 185.00, 196.00, 207.65, 
                       220.00, 233.08, 246.94, 261.63, 277.18, 293.66, 311.13, 329.63, 349.23, 369.99, 392.00, 415.30, 
                       440.00, 466.16, 493.88, 523.25, 554.37, 587.33, 622.25, 659.25, 698.46, 739.99, 783.99, 830.61, 
                       880.00, 932.33, 987.77, 1046.50, 1108.73, 1174.66, 1244.51, 1318.51, 1396.91, 1479.98, 1567.98, 1661.22};

//Quantized frequencies array from A1 to A7 with quarter tones
/*float note_freq_quarter_tones[120] = {};
for (int i = 0; i < 119; i++) {
  float x = (note_freq[i/2] + note_freq[(i+1)/2]) / 2;
  note_freq_quarter_tones[i] = x;
}
note_freq_quarter_tones[119] = 1661.22;
*/



//Functions
void set_wavetable(byte n) {
  if (n==0) {oscillator.setTable(SIN2048_DATA);}
  else if (n==1) {oscillator.setTable(TRIANGLE2048_DATA);}
  else if (n==2) {oscillator.setTable(HALFSIN2048_DATA);}
  else if (n==3) {oscillator.setTable(SAW2048_DATA);}
  else if (n==4) {oscillator.setTable(SQUARE_NO_ALIAS_2048_DATA);}
  else {oscillator.setTable(HOMEBREW_CLASH2048_DATA);}
}

/*
void read_trigger() {
  if (digitalRead(trigger_pin) == HIGH and trigger == LOW) {
    envelope.noteOn();
    trigger = HIGH;
  }
  else if (digitalRead(trigger_pin) == LOW and trigger == HIGH) {
    envelope.noteOff();
    trigger = LOW;
  }
}
*/




void setup() {
  startMozzi();
  oscillator.setFreq(220);
  oscillator.setTable(SIN2048_DATA);
  //lpf.setResonance(0);
  //lpf.setCutoffFreq(255);
  //envelope.setLevels(255, 150, 150, 0);
  //envelope.setTimes(20, 20, 4000, 20);
}




void updateControl() {
  //read inputs
  tone_index = tone_index_map(mozziAnalogRead(analog_inputs[0]));
  volume = mozziAnalogRead(analog_inputs[1]) >> 2;
  waveform = wavetable_choice_map(mozziAnalogRead(analog_inputs[2]));
  
  //read_trigger();
  
  set_wavetable(waveform);
  oscillator.setFreq(note_freq[tone_index]); 
  
  //cutoff = (mozziAnalogRead(analog_inputs[3]) >> 3) + 50;
  //resonance = mozziAnalogRead(analog_inputs[4]) >> 4;
  //lpf.setResonance(resonance);
  //lpf.setCutoffFreq(cutoff);
}




int updateAudio() {
  int audio;
  if (waveform != 3 or waveform != 4) { audio = (oscillator.next() * volume) >> 2; }
  else { audio = (oscillator.next() * volume) >> 4; } // the square and saw waves are fucking loud -- this mellows them to be about the same volume as the other waveforms
  //int filtered_audio = lpf.next(audio);
  return audio;
  //return filtered_audio;
}




void loop() {
  audioHook();
}
