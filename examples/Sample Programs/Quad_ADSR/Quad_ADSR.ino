/*This code reads the Betweener Trigger inputs and creates
   envelopes on the 4 outputs. It (mis)uses objects from the 
   Teensy Audio Design Tool, but the Teensy Audio Shield is not
   required.

   Knobs 1-4 set the attack, decay, sustain, and release.
   
   CV inputs 1-4 attenuate the envelopes.

  Example Code by Joseph Kramer - 24 August, 2018
*/


//include the Betweener library
#include <Betweener.h>

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioSynthWaveformDc     dc2;            //xy=262.45001220703125,191.4499969482422
AudioSynthWaveformDc     dc3;            //xy=262.45001220703125,227.4499969482422
AudioSynthWaveformDc     dc4;            //xy=262.45001220703125,265.45001220703125
AudioSynthWaveformDc     dc1;            //xy=263.45001220703125,153.4499969482422
AudioEffectEnvelope      envelope1;      //xy=394.4499969482422,153.4499969482422
AudioEffectEnvelope      envelope2;      //xy=395.45001220703125,191.4499969482422
AudioEffectEnvelope      envelope3;      //xy=395.45001220703125,227.4499969482422
AudioEffectEnvelope      envelope4;      //xy=399.45001220703125,263.45001220703125
AudioAnalyzeRMS          rms1;           //xy=528.4500122070312,153.4499969482422
AudioAnalyzeRMS          rms2;           //xy=528.4500122070312,190.4499969482422
AudioAnalyzeRMS          rms3;           //xy=529.4500122070312,226.4499969482422
AudioAnalyzeRMS          rms4;           //xy=531.4500122070312,262.45001220703125
AudioOutputAnalog        dac1;           //xy=698.4500122070312,452.45001220703125
AudioConnection          patchCord1(dc2, envelope2);
AudioConnection          patchCord2(dc3, envelope3);
AudioConnection          patchCord3(dc4, envelope4);
AudioConnection          patchCord4(dc1, envelope1);
AudioConnection          patchCord5(envelope1, rms1);
AudioConnection          patchCord6(envelope2, rms2);
AudioConnection          patchCord7(envelope3, rms3);
AudioConnection          patchCord8(envelope4, rms4);
// GUItool: end automatically generated code


//make a Betweener object. anytime you want to talk to the Betweener
//using the library, you will start by using the name "b."
Betweener b;


void setup() {
  //the Betweener begin function is necessary before it will do anything
  b.begin();
  
  //for the audio tool stuff
  AudioMemory(20); //needed for Audio Library

  //since the envelope is really a VCA with envelope controls, there must
  //be a steady voltage applied to the input that gets measured at the
  //output with an RMS object.
  dc1.amplitude(1);
  dc2.amplitude(1);
  dc3.amplitude(1);
  dc4.amplitude(1);

}

void loop() {

  if (rms1.available()) { //check to see if there is a new envelope value
    float env1Val = rms1.read(); //read the value of the envelope
    float scaledEnv1Val = mapFloat(env1Val, 0.0, 1.0, 0, 4095); //scale the value
    b.writeCVOut(1, scaledEnv1Val); //write the scaled value to the Betweener output1
    Serial.println(scaledEnv1Val);
  }

  if (rms2.available()) { //check to see if there is a new envelope value
    float env2Val = rms2.read(); //read the value of the envelope
    float scaledEnv2Val = mapFloat(env2Val, 0.0, 1.0, 0, 4095);
    b.writeCVOut(2, scaledEnv2Val); //write the scaled value to the Betweener output2
  }
  
  if (rms3.available()) { //check to see if there is a new envelope value
    float env3Val = rms3.read(); //read the value of the envelope
    float scaledEnv3Val = mapFloat(env3Val, 0.0, 1.0, 0, 4095);
    b.writeCVOut(3, scaledEnv3Val); //write the scaled value to the Betweener output2
  }
  
  if (rms4.available()) { //check to see if there is a new envelope value
    float env4Val = rms4.read(); //read the value of the envelope
    float scaledEnv4Val = mapFloat(env4Val, 0.0, 1.0, 0, 4095);
    b.writeCVOut(4, scaledEnv4Val); //write the scaled value to the Betweener output2
  }
  
  //this function reads just the triggers. Then we will check for certain conditions.
  b.readTriggers();

  // Check trigger for rising voltage/Note ON
  if (b.triggerRose(1)) {
    envelope1.noteOn();
  }
  if (b.triggerRose(2)) {
    envelope2.noteOn();
  }
  if (b.triggerRose(3)) {
    envelope3.noteOn();
  }
  if (b.triggerRose(4)) {
    envelope4.noteOn();
  }

  // Check each trigger for a falling voltage/Note OFF
  if (b.triggerFell(1)) {
    envelope1.noteOff();  //Begin the release phase.
  }
  if (b.triggerFell(2)) {
    envelope2.noteOff();  //Begin the release phase.
  }
  if (b.triggerFell(3)) {
    envelope3.noteOff();  //Begin the release phase.
  }
  if (b.triggerFell(4)) {
    envelope4.noteOff();  //Begin the release phase.
  }

  int attackVal = b.readKnob(1);
  envelope1.attack(attackVal);
  envelope2.attack(attackVal);
  envelope3.attack(attackVal);
  envelope4.attack(attackVal);

  int decayVal = b.readKnob(2);
  envelope1.decay(decayVal);
  envelope2.decay(decayVal);
  envelope3.decay(decayVal);
  envelope4.decay(decayVal);


  //scale the knob read from 0 - 1023 to 0.0 to 1.0 for the sustain level
  float sustainLevel = mapFloat(b.readKnob(3), 0.0, 1023.0, 0.0, 1.0);
  envelope1.sustain(sustainLevel);
  envelope2.sustain(sustainLevel);
  envelope3.sustain(sustainLevel);
  envelope4.sustain(sustainLevel);


  int releaseVal = b.readKnob(4);
  envelope1.release(releaseVal);
  envelope2.release(releaseVal);
  envelope3.release(releaseVal);
  envelope4.release(releaseVal);

  //scale all envelopes based on an inversion of the voltage on 
  //the associated CV inputs. 0V = full scale, 5V = silent
   dc1.amplitude(mapFloat(b.readCV(1), 0, 1023, 1.0, 0.));
   dc2.amplitude(mapFloat(b.readCV(2), 0, 1023, 1.0, 0.));
   dc3.amplitude(mapFloat(b.readCV(3), 0, 1023, 1.0, 0.));
   dc4.amplitude(mapFloat(b.readCV(4), 0, 1023, 1.0, 0.));
}


//funtion for mapping float ranges
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
