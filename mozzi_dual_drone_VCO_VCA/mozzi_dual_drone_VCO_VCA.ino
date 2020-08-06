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


//PINS!
const byte analog_inputs[2][5] = {{A0, A1, A2},
                                  {A3, A4, A2}};  // {tone, volume, waveform}
const byte mode_inputs[2] = {5, 6};

//global variables
int freqs[2] = {0, 0};
byte volumes[2] = {0, 0};
byte waveforms[2] = {0, 0};
bool modes[2] = {0, 0};

//oscillator
Oscil <2048, AUDIO_RATE> oscillators[2];

//maps
AutoMap freq_map(0, 1023, 55, 880);
AutoMap tone_index_map(0, 1023, 0, 48);


//Quantized frequencies array from A1 to A7
float note_freq[49] = {55.00, 58.27, 61.74, 65.41, 69.30, 73.42, 77.78, 82.41, 87.31, 92.50, 98.00, 103.83, 
                       110.00, 116.54, 123.47, 130.81, 138.59, 146.83, 155.56, 164.81, 174.61, 185.00, 196.00, 207.65, 
                       220.00, 233.08, 246.94, 261.63, 277.18, 293.66, 311.13, 329.63, 349.23, 369.99, 392.00, 415.30, 
                       440.00, 466.16, 493.88, 523.25, 554.37, 587.33, 622.25, 659.25, 698.46, 739.99, 783.99, 830.61, 
                       880.00};

//Quantized frequencies array from A1 to A7 with quarter tones



//Functions
void set_wavetable(byte n, byte osc) {
  if (n==0) {oscillators[osc].setTable(SIN2048_DATA);}
  else if (n==1) {oscillators[osc].setTable(HALFSIN2048_DATA);}
  else if (n==2) {oscillators[osc].setTable(SAW2048_DATA);}
  else if (n==3) {oscillators[osc].setTable(SQUARE_NO_ALIAS_2048_DATA);}
}

int set_freq(byte osc) {
  if (modes[osc] == 0) {
    return freq_map(mozziAnalogRead(analog_inputs[osc][0]));
  }
  else {
    float value = note_freq[tone_index_map(mozziAnalogRead(analog_inputs[osc][0]))];
    return round(value);
  }
}


void setup() {
  startMozzi();
}




void updateControl() {
  for (byte i=0; i<2; i++) {
    //read inputs
    modes[i] = digitalRead(mode_inputs[i]);
    freqs[i] = set_freq(i);
    volumes[i] = mozziAnalogRead(analog_inputs[i][1]) >> 2;
    waveforms[i] = mozziAnalogRead(analog_inputs[i][2]) >> 8;
    
    set_wavetable(waveforms[i], i);
    oscillators[i].setFreq(freqs[i]); 
  }
}




int updateAudio() {
  int audio1 = (oscillators[0].next() * volumes[0]) >> 2; 
  int audio2 = (oscillators[1].next() * volumes[1]) >> 2;
  return (audio1 + audio2) >> 1;
}




void loop() {
  audioHook();
}
