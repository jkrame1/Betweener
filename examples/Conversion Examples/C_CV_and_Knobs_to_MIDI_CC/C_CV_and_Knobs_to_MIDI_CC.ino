////////////////////////////////////////
//This code reads the Betweener's knobs and CV inputs and
//outputs corresponiding MIDI CC messages over USB.
//These Messages can be mapped in audio software such as
//Logic, Reaper, Ableton Live, Max/MSP, etc.
//
//Note, this code will only compile if your Arduino IDE
//is set so that "Tools->USB Type->[  ]" is set
//to one of the options that includes "MIDI".
//
//Joseph Kramer_June, 2018
/////////////////////////////////////////

#include <Betweener.h>

Betweener b;

// the MIDI channel number to send messages
const int channel = 1;

//create variables to assign MIDI CC channels
int CC1 = 20;
int CC2 = 21;
int CC3 = 22;
int CC4 = 23;
int CC5 = 24;
int CC6 = 25;
int CC7 = 26;
int CC8 = 27;

void setup() {
  //the Betweener begin function is necessary before it will
  //do anything
  b.begin();
  
  Serial.begin(115200);
}


void loop() {
// ask the betweener to read its analog inputs
b.readAllInputs();
 
  // only transmit MIDI messages if analog input changed
  if (b.CVChanged(1)) {
    usbMIDI.sendControlChange(CC1, b.readCVInputMIDI(1), channel);
  }
    if (b.CVChanged(2)) {
    usbMIDI.sendControlChange(CC2, b.readCVInputMIDI(2), channel);
  }
    if (b.CVChanged(3)) {
    usbMIDI.sendControlChange(CC3, b.readCVInputMIDI(3), channel);
  }
    if (b.CVChanged(4)) {
    usbMIDI.sendControlChange(CC4, b.readCVInputMIDI(4), channel);
  }

  if (b.knobChanged(1)) {
    usbMIDI.sendControlChange(CC5, b.readKnobMIDI(1), channel);
  }
   if (b.knobChanged(2)) {
    usbMIDI.sendControlChange(CC6, b.readKnobMIDI(2), channel);
  }
    if (b.knobChanged(3)) {
    usbMIDI.sendControlChange(CC7, b.readKnobMIDI(3), channel);
  }
    if (b.knobChanged(4)) {
    usbMIDI.sendControlChange(CC8, b.readKnobMIDI(4), channel);
  }
  delay(1);

  // If a MIDI Controller is not designed to respond to incoming MIDI, it
  // should discard incoming MIDI messages. Othwerwise, the controller will
  // work for a moment and then suddenly stop.
  // http://forum.pjrc.com/threads/24179-Teensy-3-Ableton-Analog-CC-causes-midi-crash
  while (usbMIDI.read()) {
    // ignore incoming messages
  }
}



