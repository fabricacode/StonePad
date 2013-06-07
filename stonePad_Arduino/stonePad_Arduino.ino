/*
STONE PAD

This code is being used for the musical instrument Stone Pad. A wonderfully designed 16 velocity-sensitive stone buttons (yes, the buttons are made of stones) midi pad. 

The instrument has been built using force sensing resistors and an Arduino Mega, for having 16 analog inputs and low latency performances. 

Analog values received from the sensors are derived by subtracting the previous value read to the current one.  This way is possible to detect edges in the signal, so trigger the note when the signal firstly reaches the top. 
When peaks are detected in the derived signal, a MIDI note with the sensed value as velocity (peak detection code from here... http://billauer.co.il/peakdet.html ). 
The signal and its derivated value are both filtered by means of exponential moving average algorithm for smoothing so avoiding errors in peak detection and for the same reason a minimum recovering time between two peaks is set.

The MIDI library used for formatting values in standard MIDI messages is included in the Arduino IDE by 2013

Open source software Hairless Midi<->Serial ( http://projectgus.github.io/hairless-midiserial/ ) is then used for routing messages coming from a serial ports to a MIDI port, thus allowing for example to use the instrument to control Ableton Live or 
any other music software with MIDI-in capabilities. 

Leonardo Amico
Fabrica 2013
*/

#include <MIDI.h>


const int nValues = 16; 

int val[nValues];
int filteredVal[nValues];
float alpha = 0.45; //the lower the more it takes to recover

//derivation
int lastVal[nValues];
int diffVal[nValues];
int filteredDiffVal[nValues];
float alpha2 = 0.3;   

//peak detection
//triggering values and sensor value dynamics are set one by one, since weight and shape of each stone are heavily affecting these values
int minDiff[] = {25, 8, 40, 30, 120, 45, 130, 10, 20, 25, 90, 70, 80, 30, 8, 15}; 
int topVal[] = {700, 900, 1023,  1023, 1000, 600, 1023, 600, 500, 700, 800, 1023, 900, 1023, 600, 1000}; 
int bottomVal[] = {50, 0, 500, 0, 100, 50,  300, 50, 20  , 100, 50, 500  , 50 , 700 , 50 , 200}; 
//
int maxVal[nValues] ; 
int minVal[nValues] ; 
boolean bLookForMax[nValues] ; 
unsigned long peakTime[nValues] ; 
unsigned long minRestoreTime = 80; //this  
//

void setup()
{
  MIDI.begin(2);// Launch MIDI with default options
  Serial.begin(115200);

  for (int i=0; i<nValues; i++)
  {
    val[i] = 0; 
    filteredVal[i] = 0; 

    lastVal[i] = 0; 
    diffVal[i] = 0; 
    filteredDiffVal[i] = 0; 

    peakTime[i] = millis(); 
    maxVal[i] =-1; 
    minVal[i] = 1024; 
    bLookForMax[i] = true; 
    
  }

}

void loop()
{


  for (int i=0; i<nValues; i++)
  {
    lastVal[i] = filteredVal[i];     
    val[i] = analogRead(i); 
    filteredVal[i] = (int)((float)val[i]*alpha + (float)filteredVal[i]*(1-alpha)); 
    diffVal[i] = filteredVal[i] - lastVal[i]; 
    filteredDiffVal[i] = (int)((float)diffVal[i]*alpha2 + (float)filteredDiffVal[i]*(1-alpha2)); 
    float processedVal = filteredDiffVal[i]; 

    //peak detection
    if (processedVal > maxVal[i]) {
      maxVal[i] =  processedVal; 
    }

    if (processedVal < minVal[i]) {
      minVal[i] =  processedVal;
    }

    if (bLookForMax[i]) {
      if (processedVal < maxVal[i] - minDiff[i]) {
        //if this happen means that I'm starting to go down after a rise
        minVal[i] = processedVal; //so the next point after a peak, if low enough, it's a new min reference
        bLookForMax[i] = false; // wait for the signal to rise enough again, before to cheking for a new peak
        if (millis() - peakTime[i] > minRestoreTime ) {
          int sensorVal = filteredVal[i]; 
        if (sensorVal > topVal[i]) sensorVal =  topVal[i]; 
          else if (sensorVal < bottomVal[i]) sensorVal = bottomVal[i]; 
          int vel = (int)(((float)(sensorVal - bottomVal[i])/(float)(topVal[i] - bottomVal[i]))*127.0); 
          int note = i; 
          MIDI.sendNoteOn(note,(int)vel,1);  // Send a Note (pitch 42, velo 127 on channel 1)
          MIDI.sendNoteOff(note,(int)vel,1);   // Stop the note   
          peakTime[i] = millis(); 
        }
      }
    }
    else {
      if (processedVal > minVal[i] + minDiff[i]) {
        //starting to rise 
        maxVal[i] = processedVal;  
        bLookForMax[i] = true; 
      }
    }

  }

}



