#include <Betweener.h>

Betweener b;

// the MIDI channel number to send messages
const int channel = 1;

// create variables to store current and previous values of knobs and CV Inputs
int  CurrentCV1, CurrentCV2, CurrentCV3, CurrentCV4 = -1;
int CurrentKnob1, CurrentKnob2, CurrentKnob3, CurrentKnob4 = -1;
int prevCV1, prevCV2, prevCV3, prevCV4 = -1;
int prevKnob1, prevKnob2, prevKnob3, prevKnob4 = -1;

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
  CurrentCV1 = b.readCVInputMIDI(1);
  CurrentCV2 = b.readCVInputMIDI(2);
  CurrentCV3 = b.readCVInputMIDI(3);
  CurrentCV4 = b.readCVInputMIDI(4);
  CurrentKnob1 = b.readKnobMIDI(1);
  CurrentKnob2 = b.readKnobMIDI(2);
  CurrentKnob3 = b.readKnobMIDI(3);
  CurrentKnob4 = b.readKnobMIDI(4);


  // only transmit MIDI messages if analog input changed
  if (CurrentCV1 != prevCV1) {
    usbMIDI.sendControlChange(CC1, CurrentCV1, channel);
    prevCV1 = CurrentCV1;
  }
  if (CurrentCV2 != prevCV2) {
    usbMIDI.sendControlChange(CC2, CurrentCV2, channel);
    prevCV2 = CurrentCV2;
  }
  if (CurrentCV3 != prevCV3) {
    usbMIDI.sendControlChange(CC3, CurrentCV3, channel);
    prevCV3 = CurrentCV3;
  }
  if (CurrentCV4 != prevCV4) {
    usbMIDI.sendControlChange(CC4, CurrentCV4, channel);
    prevCV4 = CurrentCV4;
  }

  if (CurrentKnob1 != prevKnob1) {
    usbMIDI.sendControlChange(CC5, CurrentKnob1, channel);
    prevKnob1 = CurrentKnob1;
  }
  if (CurrentKnob2 != prevKnob2) {
    usbMIDI.sendControlChange(CC6, CurrentKnob2, channel);
    prevKnob2 = CurrentKnob2;
  }
  if (CurrentKnob3 != prevKnob3) {
    usbMIDI.sendControlChange(CC7, CurrentKnob3, channel);
    prevKnob3 = CurrentKnob3;
  }
  if (CurrentKnob4 != prevKnob4) {
    usbMIDI.sendControlChange(CC8, CurrentKnob4, channel);
    prevKnob4 = CurrentKnob4;
  }

  delay(50);

  // MIDI Controllers should discard incoming MIDI messages.
  // http://forum.pjrc.com/threads/24179-Teensy-3-Ableton-Analog-CC-causes-midi-crash
  while (usbMIDI.read()) {
    // ignore incoming messages
  }
}



