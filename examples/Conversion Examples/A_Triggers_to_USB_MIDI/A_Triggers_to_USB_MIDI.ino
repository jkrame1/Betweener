/*This code reads the Betweener Trigger inputs and sends
  corresponding USB MIDI Note On/Off messages.

  Note, this code will only compile if your Arduino IDE
  is set so that "Tools->USB Type->[  ]" is set
  to one of the options that includes "MIDI".

  Example Code by Joseph Kramer - 20 June, 2018
*/


//include the Betweener library
#include <Betweener.h>

//make a Betweener object. anytime you want to talk to the Betweener
//using the library, you will start by using the name "b."
Betweener b;

const int channel = 1;

void setup() {
  //the Betweener begin function is necessary before it will do anything
  b.begin();

}

void loop() {

  //this function reads just the triggers. We could also do readAllInputs().
  b.readTriggers();

  // Check each trigger for rising voltage/Note ON
  if (b.triggerRose(1)) {
    usbMIDI.sendNoteOn(60, 127, channel);  // 60 = C4
  }
  if (b.triggerRose(2)) {
    usbMIDI.sendNoteOn(62, 127, channel);  // 62 = D4
  }
  if (b.triggerRose(3)) {
    usbMIDI.sendNoteOn(64, 127, channel);  // 64 = E4
  }
  if (b.triggerRose(4)) {
    usbMIDI.sendNoteOn(65, 127, channel);  // 65 = F4
  }

  // Check each trigger for a falling voltage/Note OFF
  if (b.triggerFell(1)) {
    usbMIDI.sendNoteOff(60, 0, channel);  // 60 = C4
  }
  if (b.triggerFell(2)) {
    usbMIDI.sendNoteOff(62, 0, channel);  // 62 = D4
  }
  if (b.triggerFell(3)) {
    usbMIDI.sendNoteOff(64, 0, channel);  // 64 = E4
  }
  if (b.triggerFell(4)) {
    usbMIDI.sendNoteOff(65, 0, channel);  // 65 = F4
  }

   // If a MIDI Controller is not designed to respond to incoming MIDI, it
  // should discard incoming MIDI messages. Othwerwise, the controller will
  // work for a moment and then suddenly stop.
  // http://forum.pjrc.com/threads/24179-Teensy-3-Ableton-Analog-CC-causes-midi-crash
  while (usbMIDI.read()) {
    // ignore incoming messages
  }
}
