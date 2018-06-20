///////////////////////////////////////////////////////////
//USB_MIDI_Note_to_CV
//
//This code receives MIDI over USB from a computer and generates Control Voltages
//on the four Betweener outputs.
//  1 Note to CV - this is roughly 1v/oct
//  2 Gate - High for Note On Messages and Low for Note Off messages (or velocity of 0)
//  3 Velocity CV - very wide range, can be remapped in NOTE ON function below
//  4 Aftertouch CV - mapped for full voltage output range
//
//includes portions of PJRC Teensy MIDI examples
//Uses the Betweener Library by Kathryn Schaffer
//
//Example Code by Joseph Kramer - 5 Feb 2018
///////////////////////////////////////////////////////////

#include <Betweener.h>

Betweener b;

// the MIDI channel number to send messages
const int channel = 1;

//Create a variable to keep track of the most recent note. This keeps
//the gate from being set low by relased notes that were held down past
//the onset of the most recent note
byte recentNote;

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
  
  int noteCV = map(note, 60, 120, 0, 4095); //turn noteON messages into a CV range
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



