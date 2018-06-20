///////////////////////////////////////////////////////////
//USB_MIDI_Note_to_CV
//
//This code receives MIDI over USB from a computer and generates Control Voltages
//on the four Betweener outputs:
//  1 CC 28 to CV1
//  2 CC 29 to CV2
//  3 CC 30 to CV3
//  4 CC 31 to CV4
//
//includes portions of PJRC Teensy MIDI examples
//Uses the Betweener Library by Kathryn Schaffer
//
//Example Code by Joseph Kramer - 7 March 2018
///////////////////////////////////////////////////////////

#include <Betweener.h>

Betweener b;

// the MIDI channel number to send messages
const int channel = 1;


void setup() {
  b.begin();  //start the Betweener
  Serial.begin(115200);
  usbMIDI.setHandleControlChange(OnControlChange);  //
  pinMode(8, OUTPUT); // Set Pin 8, attached to the Betweener LED, to an Output
}


void loop() {
  usbMIDI.read(); // USB MIDI receive
}


void OnControlChange(byte channel, byte control, byte value) {
  Serial.print("Control Change, ch=");
  Serial.print(channel, DEC);
  Serial.print(", control=");
  Serial.print(control, DEC);
  Serial.print(", value=");
  Serial.print(value, DEC);
  Serial.println();

  switch (control) {
    case  28: //if the controller number is 28, do what is between here and "break;"
      b.writeCVOut(1, b.MIDItoCV(value)); //Scale the 7bit MIDI value to a 12bit DAC value and write it as a CV
      break;

    case 29:
      b.writeCVOut(2, b.MIDItoCV(value));
      break;

    case 30:
      b.writeCVOut(3, b.MIDItoCV(value));
      break;

    case 31:
      b.writeCVOut(4, b.MIDItoCV(value));
      break;

      default:
      break;
      
  }

}



