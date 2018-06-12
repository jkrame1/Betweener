#include <Betweener.h>

Betweener b;

// the MIDI channel number to send messages
const int channel = 1;

//create variables for MIDI CC channels
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

  // MIDI Controllers should discard incoming MIDI messages.
  // http://forum.pjrc.com/threads/24179-Teensy-3-Ableton-Analog-CC-causes-midi-crash
  while (usbMIDI.read()) {
    // ignore incoming messages
  }
}



