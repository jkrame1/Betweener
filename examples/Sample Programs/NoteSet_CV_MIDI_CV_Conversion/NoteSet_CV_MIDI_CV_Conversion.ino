///////////////////////////////////////////////////////////
//Triggers_CV_Knobs_to_USB_MIDI_|_USB_MIDI_Note_to_CV_Gate_Velocity_Aftertouch
//
//This code sends MIDI notes and CCs over USB that correspond to the four
//knobs, CV inputs, and Triggers. It also receives MIDI over USB
//from a computer and generates Control Voltages on the four
//Betweener outputs.
//  1 Note to CV - this is roughly 1v/oct
//  2 Gate - HIGH(5v) while key(s) pressed and Low (0V) when all keys are released
//  3 Velocity CV - very wide range, can be remapped in NOTE ON function below
//  4 Aftertouch CV - mapped for full voltage output range
//
//Includes portions of PJRC Teensy MIDI examples
//Uses the Betweener Library by Kathryn Schaffer
//Uses the NoteSet Library by Kathryn Schaffer that adapts
//the note-handling algorithm written by émilie gillet for mutable insturments
//Download NoteSet here: https://github.com/kschaffer/NoteSet
//
//Example Code by Joseph Kramer - 29 MARCH 2019
///////////////////////////////////////////////////////////

#include <NoteSet.h>
#include <Betweener.h>

Betweener b;
NoteSet set;

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
  //options for the priority are NOTE_PRIORITY_LAST
  //NOTE_PRIORITY_HIGH, and NOTE_PRIORITY_LOW
  //this determines how the note set will pick which
  //note to default to when other notes are played or
  //released simultaneously.
  set.init(NOTE_PRIORITY_LAST);

  b.begin();  //start the Betweener

  Serial.begin(115200);

  //We are defining our own "callback" functions to use
  //NoteSet to handle note on and note off messages
  //from USB MIDI
  usbMIDI.setHandleNoteOn(myNoteOn);
  usbMIDI.setHandleNoteOff(myNoteOff);
  usbMIDI.setHandleAfterTouch(myAfterTouch);
  pinMode(8, OUTPUT); // Set Pin 8, attached to the Betweener LED, to an Output
}


void loop() {
  b.readUsbMIDI();

  ///////////////////////////////////////////////////
  //this function reads just the triggers.
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

  //////////////////////////////////////////////////////////////

  //CVInputs and KNOBS to USB MIDI Continous Controller (CC) messages/////////
  // only transmit MIDI messages if analog input changed
  if (b.CVChanged(1)) {
    usbMIDI.sendControlChange(CC1, b.readCVInputMIDI(1), channel);  //CC#, CCvalue between 0-127, MIDI channel
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

  //////////////////////////////////////////////////////////////////////

}

//callback functions:

void myNoteOn(byte channel, byte note, byte velocity) {

  //load up the new note into our note set
  set.addNote(note, velocity);

  //ask the note set which note to play next
  //(which depends on the setting for the priority)
  NoteInfo note_to_play = set.getNextNote();


  //if an empty note is returned, we shouldn't play it!
  if (note_to_play.note != EMPTY_NOTE) {
    //Serial.println(String("MIDI note:  ") + note_to_play.note + String(" velocity ") + note_to_play.velocity);

    //***HERE IS WHERE WE PLAY SOMETHING:
    int noteCV = map(note_to_play.note, 24, 84, 0, 4095); //turn noteON messages from C1(24)-C6(84) into a 5-volt CV range
    b.writeCVOut(1, noteCV); //When a note-on is received, write a CV value on output 1 scaled to the note value

  }
  if (velocity == 0) {
    b.writeCVOut(2, 0); //When a note-off is received, write a gate LOW on CVOUT 2
    digitalWrite(8, LOW); //turn LED off if no Gate is present
  }

  else {
    b.writeCVOut(2, 4095); //When a note-on is received, write a gate HIGH on CVOUT 2
    digitalWrite(8, HIGH); //turn LED On when a Note On message is received
  }

  //NOTE, This code only passes the most recent velocity, not the velocity reported by the
  //note_set algorithm. This is by design. If the orignal note velocity is desired, then in the
  //following lines simply replace "velocity" with "note_to_play.velocity"
  int velocityCV = map(velocity, 0, 127, 100, 4095); //scale MIDI velocity values to 12bit range for CV output
  b.writeCVOut(3, velocityCV); //Write velocity CV on CVOUT 3

}

void myNoteOff(byte channel, byte note, byte velocity) {
  //make sure the note gets removed from the note set
  set.removeNote(note);

  //When playing a keyboard it is common to hold a key while pressing then releasing
  //another key. This means that note-off messages will happen while keys are still being pressed.
  //This could mean that a note off message indicates that we need to trigger playing a new (old) note.
  //check if that's the case
  NoteInfo note_to_play = set.getNextNote();
  if (note_to_play.note != EMPTY_NOTE) {
    //Serial.println(String("MIDI note:  ") + note_to_play.note + String(" velocity ") + note_to_play.velocity);

    //***HERE IS WHERE WE PLAY SOMETHING:
    int noteCV = map(note_to_play.note, 24, 84, 0, 4095); //turn noteON messages into a CV range
    b.writeCVOut(1, noteCV); //When a note-on is received, write a CV value scaled to the note value

  }
  if (!set.get_size()) {  //get_size() will return how many notes are still held down. When the last is let up, this will return 0
    b.writeCVOut(2, 0); //When the last note-off is received, write a gate LOW on CVOUT 2
    digitalWrite(8, LOW); //Turn LED off when a Note Off is received
  }

}

//AFTERTOUCH - CV Output on CVOUT 4
void myAfterTouch(byte channel, byte pressure) {
  int pressureCV = map(pressure, 0, 127, 0, 4095);
  b.writeCVOut(4, pressureCV);
}
