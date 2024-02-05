I love noise. A lot. In pursuit of deeply textured noise machines, I've been tinkering around with lo-fi digital synthesis for a little while now. Unfortunately, most of them are, well, limited. Most of my attempts have either hit the deeply textured noise goal but at the cost of all musicality, or are capable of only the merest beeps, boops, and warbles. MCUs only have so many clock cycles and pins to play around with, and they get used up super quickly when designing little noise machines. Which is where modular comes in.

Like any good lover of noise and aspiring maker of machines that make it, I think modular synthesizers are radical. They are also prohibitively expensive. I'm a stay-at-home dad with very (very) little disposable income so something that costs $300 and doesn't even make any noise until I've also purchased a handful of other $300 things isn't really an option. On the flip side, the DIY modular systems I come across, while extremely cool and inspirational, always seem more complicated than necessary by avoiding MCUs: requiring a dozen or so different ICs and (sometimes) nontrivial peripheral circuitry. So, I wanted to make a system that was suuuuuuper simple, requiring just a handful of the same parts in each module and ridiculously basic peripheral circuits.

Et voila!

All together, the tested modules include the following:

    square wave generator
    envelope generator
    VCO with VCA
    LFO
    attenuverter with offset
    sample- or track-and-hold
    quantizer
    sequencer with two modes (step and Euclidean).

Hopefully someone out there will find these useful and/or inspirational for their own project.

-----------------------------------

NOTES:
- To get the VCO to play properly you'll need the Mozzi library by Tim Barrass (https://sensorium.github.io/Mozzi/) and you'll have to set it to Hi-Fi mode. To do so, open the mozzi_config header file and uncomment this line #define AUDIO_MODE HIFI and then comment out whichever other one was set.
    
- To get the sequencer to work you'll need three libraries: the Adafruit_NeoPixel library (https://learn.adafruit.com/adafruit-neopixel-uberguide/arduino-library-installation), Paul Stoffregan's Encoder library (https://github.com/PaulStoffregen/Encoder), and version 1.4.2 of the AceButton library (apparently the newer version pops errors at the moment).
    
- The passive low-pass filter circuits on the PWM outputs are the wrong version in the schematics. They should use a 1K resistor and a 0.1uF capacitor. The ones in the schematic won't attenuate the PWM frequencies that the current code uses so are basically useless.
    
- The VCO uses a half-sine wavetable that I made myself so the code throws an error. You can either replace that wavetable for another one that comes standard with Mozzi, or you can add the half-sine wavetable to the Mozzi library "tables" folder. It can be downloaded at my github here: https://github.com/scraptured/DIY_modular_Arduino/tree/master/mozzi_dual_mode_VCO_with_VCA
