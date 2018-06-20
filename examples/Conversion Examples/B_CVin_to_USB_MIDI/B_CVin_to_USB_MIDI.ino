/*This code reads the Betweener CV inputs and sends
  corresponding USB CC messages only when the volatage changes
*/


//include the Betweener library
#include <Betweener.h>

//make a Betweener object. anytime you want to talk to the Betweener
//using the library, you will start by using the name "b."
Betweener b;

const int channel = 1;

//These are varialbles that will be modified by our program
//as it runs
int prevCV1, prevCV2, prevCV3, prevCV4 = -1;
int  CurrentCV1, CurrentCV2, CurrentCV3, CurrentCV4 = -1;

//create variables for MIDI CC channels
int CC1 = 20;
int CC2 = 21;
int CC3 = 22;
int CC4 = 23;

void setup() {
  //the Betweener begin function is necessary before it will do anything
  b.begin();

}

void loop() {

  //Read and scale the CV Inputs. We could also do readAllInputs() or
  //b.readCVInputs(); and then scale manually
 CurrentCV1 = b.readCVInputMIDI(1);
 CurrentCV2 = b.readCVInputMIDI(2);
 CurrentCV3 = b.readCVInputMIDI(3);
 CurrentCV4 = b.readCVInputMIDI(4);
  
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


  delay(5);

  // MIDI Controllers should discard incoming MIDI messages.
  // http://forum.pjrc.com/threads/24179-Teensy-3-Ableton-Analog-CC-causes-midi-crash
  while (usbMIDI.read()) {
    // ignore incoming messages
  }
}
