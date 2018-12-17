///////////////////////////////////////////////////////////
//USB_MIDI_Note_to_Trigs
//
//This code receives MIDI on Channel 10 over USB and generates
//trigger pulses on the four Betweener outputs.
//  1 Tiggers from Note numbers 35 and 36 on channel 10
//  2 Tiggers from Note numbers 37 and 38 on channel 10
//  3 Tiggers from Note numbers 42 on channel 10
//  4 Tiggers from Note numbers 44 and 46 on channel 10
//
//includes portions of PJRC Teensy MIDI examples
//Uses the Betweener Library by Kathryn Schaffer
//
//Example Code by Joseph Kramer - 8 March 2018
///////////////////////////////////////////////////////////

#include <Betweener.h>

Betweener b;

//variable to set trigger length
int trigTime1 = 15;
int trigTime2 = 15;
int trigTime3 = 15;
int trigTime4 = 15;

void setup() {
  b.begin();  //start the Betweener
  usbMIDI.setHandleNoteOn(OnNoteOn);  //tell USB MIDI what to look for when it sees a Note On signal
  pinMode(8, OUTPUT); // Set Betweener LED to an Output
}


void loop() {
  usbMIDI.read(); // USB MIDI receive
}



//NOTE ON function - looks for common drum mappings on channel ten and writes triggers to the CV outs
void OnNoteOn(byte channel, byte note, byte velocity) {
  if (channel == 10) {
    if (note == 35 || note == 36) {
      trigOut(1);
    }

    if (note == 37 || note  == 38) {
      trigOut(2);
    }

    if (note == 42) {
      trigOut(3);
    }

    if (note == 44 || note  == 46) {
      trigOut(4);
    }
  }
}

void trigOut(byte out) {  //Tells the Betweener how to write Trigger to the CV outputs
  switch (out) {

    case 1:
      b.writeCVOut(1, 4095); //write the CV HIGH (approx 5v)
      digitalWrite(8, HIGH); //turn LED On when a Note On message is received
      delay(trigTime1); //write the CV LOW (0v)
      digitalWrite(8, LOW); //turn LED Off
      b.writeCVOut(1, 0);
      break;

    case 2:
      b.writeCVOut(2, 4095);
      digitalWrite(8, HIGH); //turn LED On when a Note On message is received
      delay(trigTime2);
      digitalWrite(8, LOW); //turn LED Off
      b.writeCVOut(2, 0);
      break;

    case 3:
      b.writeCVOut(3, 4095);
      digitalWrite(8, HIGH); //turn LED On when a Note On message is received
      delay(trigTime3);
      digitalWrite(8, LOW); //turn LED Off
      b.writeCVOut(3, 0);
      break;

    case 4:
      b.writeCVOut(4, 4095);
      digitalWrite(8, HIGH); //turn LED On when a Note On message is received
      delay(trigTime4);
      digitalWrite(8, LOW); //turn LED Off
      b.writeCVOut(4, 0);
      break;

    default:
      break;
  }
}
