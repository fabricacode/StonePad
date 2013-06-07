StonePad
========

This Arduino code is being used for the musical instrument Stone Pad. A wonderfully designed 16 velocity-sensitive stone buttons (yes, the buttons are made of stones) midi pad. 

The instrument has been built using force sensing resistors and an Arduino Mega, for having 16 analog inputs and low latency performances. 

Analog values received from the sensors are derived by subtracting the previous value read to the current one.  This way is possible to detect edges in the signal, so trigger the note when the signal firstly reaches the top. 
When peaks are detected in the derived signal, a MIDI note with the sensed value as velocity (peak detection code from here... http://billauer.co.il/peakdet.html ). 
The signal and its derivated value are both filtered by means of exponential moving average algorithm for smoothing so avoiding errors in peak detection and for the same reason a minimum recovering time between two peaks is set.

The MIDI library used for formatting values in standard MIDI messages is included in the Arduino IDE by 2013

Open source software Hairless Midi<->Serial ( http://projectgus.github.io/hairless-midiserial/ ) is then used for routing messages coming from a serial ports to a MIDI port, thus allowing for example to use the instrument to control Ableton Live or 
any other music software with MIDI-in capabilities. 

Leonardo Amico
Fabrica 2013
