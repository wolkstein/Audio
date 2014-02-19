// DAC balance example: Will influence both HP & LO outputs.

#include <Audio.h>
#include <Wire.h>
#include <SD.h>


const int myInput = AUDIO_INPUT_LINEIN;
// const int myInput = AUDIO_INPUT_MIC;

// Create the Audio components.  These should be created in the
// order data flows, inputs/sources -> processing -> outputs
//

AudioInputI2S       audioInput;         // audio shield: mic or line-in
AudioOutputI2S      audioOutput;        // audio shield: headphones & line-out

// Create Audio connections between the components
//
AudioConnection c1(audioInput, 0, audioOutput, 0); // left passing through
AudioConnection c2(audioInput, 1, audioOutput, 1); // right passing through

// Create an object to control the audio shield.
// 
AudioControlSGTL5000 audioShield;

void setup() {
  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(4);
  // Enable the audio shield and set the output volume.
  audioShield.enable();
  audioShield.inputSelect(myInput);
  audioShield.volume(75);
  audioShield.unmuteLineout();
}

elapsedMillis chgMsec=0;
float lastBal=1024;

void loop() {
  // every 10 ms, check for adjustment the balance & vol
  if (chgMsec > 10) { // more regular updates for actual changes seems better.
    float bal1=analogRead(15);
    bal1=((bal1-512)/512)*100;
    bal1=(int)bal1;
    if(lastBal!=bal1)
    {
      if(bal1<0)
      { // leaning toward left...
        audioShield.dac_vol(100,100+bal1);
      } else if(bal1>0) { // to the right
        audioShield.dac_vol(100-bal1,100);
      } else { // middle
        audioShield.dac_vol(100);
      }
      lastBal=bal1;
    }
    chgMsec = 0;
  }
}
