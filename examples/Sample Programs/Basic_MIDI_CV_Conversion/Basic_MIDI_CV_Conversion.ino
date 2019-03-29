///////////////////////////////////////////////////////////
//CV_to_USB_MIDI_|_USB_MIDI_Note_to_CV
//
//This code sends MIDI notes and CCs over USB that correspond to the four 
//knobs, CV inputs, and Triggers. It also receives MIDI over USB 
//from a computer and generates Control Voltages on the four 
//Betweener outputs.
//  1 Note to CV - this is roughly 1v/oct
//  2 Gate - High for Note On Messages and Low for Note Off messages (or velocity of 0)
//  3 Velocity CV - very wide range, can be remapped in NOTE ON function below
//  4 Aftertouch CV - mapped for full voltage output range
//
//includes portions of PJRC Teensy MIDI examples
//Uses the Betweener Library by Kathryn Schaffer
//
//Example Code by Joseph Kramer - 19 JUN 2018
///////////////////////////////////////////////////////////

#include <Betweener.h>

Betweener b;

// the MIDI channel number to send messages
const int channel = 1;

//Create a variable to keep track of the most recent note. This keeps
//the gate from being set low by relased notes that were held down past
//the onset of the most recent note
byte recentNote;

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
  b.begin();  //start the Betweener

  Serial.begin(115200);

  usbMIDI.setHandleNoteOff(OnNoteOff);
  usbMIDI.setHandleNoteOn(OnNoteOn);
  usbMIDI.setHandleAfterTouch(OnAfterTouch);

  pinMode(8, OUTPUT); // Set Pin 8, attached to the Betweener LED, to an Output
}


void loop() {
  usbMIDI.read(); // USB MIDI receive

 //this function reads just the triggers. We could also do readAllInputs().
  b.readTriggers();

//TRIGGERS to USB MIDI NOTES////////////////////////////////////
  // Check each button for gate HIGH rising edge/Note ON
  if (b.trig1.risingEdge()) {
    usbMIDI.sendNoteOn(60, 127, channel);  // 60 = C4
  }
  if (b.trig2.risingEdge()) {
    usbMIDI.sendNoteOn(62, 127, channel);  // 62 = D4
  }
  if (b.trig3.risingEdge()) {
    usbMIDI.sendNoteOn(64, 127, channel);  // 64 = E4
  }
  if (b.trig4.risingEdge()) {
    usbMIDI.sendNoteOn(65, 127, channel);  // 65 = F4
  }

  // Check each button for gate LOW falling edge/Note OFF
  if (b.trig1.fallingEdge()) {
    usbMIDI.sendNoteOff(60, 0, channel);  // 60 = C4
  }
  if (b.trig2.fallingEdge()) {
    usbMIDI.sendNoteOff(62, 0, channel);  // 62 = D4
  }
  if (b.trig3.fallingEdge()) {
    usbMIDI.sendNoteOff(64, 0, channel);  // 64 = E4
  }
  if (b.trig4.fallingEdge()) {
    usbMIDI.sendNoteOff(65, 0, channel);  // 65 = F4
  }
//////////////////////////////////////////////////////////////

//CVInputs and KNOBS to USB MIDI CCs//////////////////////////
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

//////////////////////////////////////////////////////////////////////
  
  
}


//NOTE ON - Sets a relative Pitch CV Out on CVOUT 1
//Gate HIGH on CVOUT 2 for any note with non-zero velocity
//Gate LOW for Note On messages with a value on zero (often used as Note Off) (corresponding only 
//to the most recent Note On message recieved)
//Velocity value is scaled and output on CVOUT 3
void OnNoteOn(byte channel, byte note, byte velocity) {
  if (velocity >= 1) {
  recentNote = note;  //set the most recent note if it is a NoteOn message
  }
  
  int noteCV = map(note, 24, 84, 0, 4095); //turn noteON messages into a CV range
  b.writeCVOut(1, noteCV); //When a note-on is received, write a CV value scaled to the note value

  if (velocity == 0 && note == recentNote) {
    b.writeCVOut(2, 0); //When a note-off is received, write a gate LOW on CVOUT 2
    digitalWrite(8, LOW); //turn LED off if no Gate is present
  }
  else {
    b.writeCVOut(2, 4095); //When a note-on is received, write a gate HIGH on CVOUT 2
    digitalWrite(8, HIGH); //turn LED On when a Note On message is received
  }

  int velocityCV = map(velocity, 0, 127, 10, 4095); //scale MIDI velocity values to 12bit range for CV output
  b.writeCVOut(3, velocityCV); //Write velocity CV on CVOUT 3 
}


//NOTE OFF - Sets Gate LOW on CVOUT 2 if a Note Off message is received
void OnNoteOff(byte channel, byte note, byte velocity) {
  if (note == recentNote){
  b.writeCVOut(2, 0); //When a note-off is received, write a gate LOW on CVOUT 2
  digitalWrite(8, LOW); //Turn LED off when a Note Off is received
  }
}


//AFTERTOUCH - CV Out on CVOUT 4
void OnAfterTouch(byte channel, byte pressure) {
  int pressureCV = map(pressure, 0, 127, 0, 4095);
  b.writeCVOut(4, pressureCV);
}
