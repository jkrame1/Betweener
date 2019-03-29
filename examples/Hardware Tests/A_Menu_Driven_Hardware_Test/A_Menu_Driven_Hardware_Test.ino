
// A_Menu_Driven_Hardware_Test

//This is a simple hardware test sketch that uses the Serial monitor as a menu interface
//for trying out the main functionality of the Betweener device.  You will need to have the 
//Betweener library and its dependencies installed properly to to compile this sketch.  

//Note that this code will not do anything until you open the Serial monitor and 
//make sure it's set to the right baud rate (115200).  Hit any key on your computer 
//and it will give you menu options for testing the Betweener functionality

#include <Betweener.h>


//make a Betweener object
Betweener b;


void setup() {
  //set up serial communication - make sure your serial monitor is set to the same
  //baud rate
  Serial.begin(115200);
  Serial.println("Press any key and hit send (or return) to begin.");
  while (!Serial.available()){
    //Do nothing until something is received over the serial port
  }

  //the Betweener begin function is necessary before it will do anything
  b.begin();

  printMenu();

}

//variable to store the keyboard input from the user
int selection;

void loop() {

  //wait for the user to input something and then grab it
  if (Serial.available()){
    //grab first character that was input
    selection=Serial.read();
    //any other characters that were input are extras (e.g. newlines) so 
    //we read them just to flush them from the input buffer
    while(Serial.available()){
      Serial.read();
    }


    //based on the user selection, different functions
    //will be run
    switch (selection){
      case '1':
        Serial.println("you entered 1.  Testing knobs.");
        printKnobValues();
        break;
      case '2':
        Serial.println("you entered 2.  Testing triggers.");
        printTriggers();
        break;
      case '3': 
        Serial.println("you entered 3.  Testing CV inputs.");
        printCVInputs();
        break;
      case '4':
        Serial.println("you entered 4.  Testing CV outputs.");
        rampCVOuts();
        break;
      case '5':
        Serial.println("you entered 5.  Testing DIN MIDI input.");
        printDINMIDI();
        break;
      case '6':
        Serial.println("you entered 6.  Testing DIN MIDI output.");
        rampMIDICC();
        break;
      case '7':
        Serial.println("you entered 7.  Testing sending knob values to CV outs");
        knobsToCV();
        break;
      case '8': 
        Serial.println("you entered 8.  Testing for knob changes");
        checkKnobChanges();
        break;
      case '9':
        Serial.println("you entered 9.  Testing reading in raw knob values");
        printKnobValuesRaw();
        break;
      default:
        //typing any key besides the numbers 1-9 will get you back
        //to the menu
        printMenu();
        break;

     }

  }


}

void printMenu(){
  Serial.println("=======================");
  Serial.println("Betweener tests.  Select from the menu below to begin a test.");
  Serial.println("Send any keystroke to stop the test and return to this menu.");
  Serial.println("");
  Serial.println("Enter 1 to test knobs");
  Serial.println("Enter 2 to test trigger inputs");
  Serial.println("Enter 3 to test CV inputs");
  Serial.println("Enter 4 to test CV outputs");
  Serial.println("Enter 5 to test DIN MIDI input (assumes you have compiled with MIDI functionality)");
  Serial.println("Enter 6 to test DIN MIDI output (assumes you have compiled with MIDI functionality)");
  Serial.println("Enter 7 to test sending knob values to CV outs (e.g. knob 1 to CV out 1, etc.)");
  Serial.println("Enter 8 to test for changes in knob value");
  Serial.println("Enter 9 to test raw knob values (not smoothed)");
  Serial.println("=======================");
  Serial.println("");
  Serial.println("");

}

void printKnobValues(){
 
  //as long as the user has not entered something in the serial monitor window,
  //we will read and print knob values
  while (!Serial.available()){
 
    /*
    //There are two ways to do this. We are leaving one commented out for reference    
    //first way reads the info into betweener and then grabs the 
    //member variables themselves
    b.readKnobs();
    //now the current knob values are stored in the betweener object
    //and we access and print them:
    Serial.println("knob values:");
    Serial.println(b.currentKnob1);
    Serial.println(b.currentKnob2);
    Serial.println(b.currentKnob3);
    Serial.println(b.currentKnob4);
    */

    //second way performs a read of each channel only when we ask for it.
    //Note that if we want these formatted to MIDI outputs we can 
    //use readKnobMIDI instead of readKnob
    Serial.println("knob values:");
    Serial.println(b.readKnob(1));
    Serial.println(b.readKnob(2));
    Serial.println(b.readKnob(3));
    Serial.println(b.readKnob(4));
    
    delay(300);
  }
}

void printTriggers(){
  while(!Serial.available()){

  b.readTriggers();   

  for (int i = 1; i<5; i++){

    if (b.triggerRose(i)){
      Serial.println(String("Trigger ")+i+String(" rose"));
      
    }
    if (b.triggerFell(i)){
      Serial.println(String("Trigger ")+i+String(" fell"));

   
    }

  }

}
}

void printCVInputs(){

  //as long as the user is not inputting anything from the keyboard,
  //we will read and print CV values with some delay
  while(!Serial.available()){

    /* 
    //There are two ways to do this.  One commented out just for 
    //reference in this code.
    //first way reads all the CV inputs all at once into Betweener
    //and then we access the current values as member variables
    b.readCVs(); 
    //now the current CV values are stored in the betweener class, and 
    //we can grab them and print them

    Serial.println("Current CV input values:");
    Serial.println(b.currentCV1);
    Serial.println(b.currentCV2);
    Serial.println(b.currentCV3);
    Serial.println(b.currentCV4);
    */

  //second way performs a read of each channel only when 
  //we ask for that channel's info.
  //note that if we want this formatted to MIDI
  // we could replace with readCVMIDI.
  Serial.println("Current CV input values:");
  Serial.println(b.readCV(1));
  Serial.println(b.readCV(2));
  Serial.println(b.readCV(3));
  Serial.println(b.readCV(4));
  

   delay(200);
  }
}

void rampCVOuts(){
  //the range of the CV outs is 0 to 4095
  int maxval=4095;
  int i=0;

  //as long as the user does not input anything, we will ramp the 
  //CV outs all together
  while(!Serial.available()){
    //write the same value out to each channel
    b.writeCVOut(1, i);
    b.writeCVOut(2, i);
    b.writeCVOut(3, i);
    b.writeCVOut(4, i);
    //this is a little silly, but we'll report what we are doing to the screen
    Serial.println("Writing output values: ");
    Serial.println(i);
    Serial.println(i);
    Serial.println(i);
    Serial.println(i);
    i=i+100;
    delay(250);
    if (i >= maxval){
      i = 0;
    }
  }
  
}

void printDINMIDI(){
   //as long as the user has not input anything, print some information from incoming MIDI messages
   while(!Serial.available()){
    //see documentation for the Arduino MIDI library to understand what these functions are doing
    if(b.DINMIDI.read()){
      Serial.println(String("MIDI message type:  ")+b.DINMIDI.getType()+String(" with data values: ")+b.DINMIDI.getData1()+String(" and ")+b.DINMIDI.getData2());
    }
    
   }


  
}

void rampMIDICC(){
  //here we're going to take MIDI CC channel 10 and ramp it
  //from 0 to 127 (on MIDI channel 1)
  int i=0;
  int cc_channel=10;
  int midi_channel=1;
  int max = 127;
  
  while(!Serial.available()){
    b.DINMIDI.sendControlChange(i,cc_channel,midi_channel);
    Serial.println(String("Sending this value to MIDI CC channel 10: ")+i);
    delay(500);
    i=i+1;
    if (i == max){
      i = 0;      
    }
  }
}

void knobsToCV(){
  //as long as the user does not interrupt with a keyboard input, take 
  //knob settings and scale them and output them
  while(!Serial.available()){
    //the betweener library has a function that reads a knob value and returns that 
    //value scaled to the right range for CV outs.  We then just write that to the 
    //channel we want.
    int val1=b.readKnobCV(1);
    int val2=b.readKnobCV(2);
    int val3=b.readKnobCV(3);
    int val4=b.readKnobCV(4);
   
    b.writeCVOut(1, val1);
    b.writeCVOut(2, val2);
    b.writeCVOut(3, val3);
    b.writeCVOut(4, val4);

    //print out to serial just for reassurance
    Serial.println(String("Writing value ")+val1+String(" to CV 1"));
    Serial.println(String("Writing value ")+val2+String(" to CV 2"));
    Serial.println(String("Writing value ")+val3+String(" to CV 3"));
    Serial.println(String("Writing value ")+val4+String(" to CV 4"));
    
    delay(250);
    
  }
}

void checkKnobChanges(){
  while(!Serial.available()){
    b.readKnobs();
    Serial.println("Knobs changing?");
    Serial.println(b.knobChanged(1));
    Serial.println(b.knobChanged(2));
    Serial.println(b.knobChanged(3));
    Serial.println(b.knobChanged(4));

    delay(400);
  }
}

void printKnobValuesRaw(){
 
  //as long as the user has not entered something in the serial monitor window,
  //we will read and print knob values
  while (!Serial.available()){
 
    //Two ways to do this; leaving one commented out for reference    
    //first way reads the info into betweener and then grabs the 
    //member variables themselves

    //second way performs a read of each channel only when we ask for it.
    //Note that if we want these formatted to MIDI outputs we can 
    //use readKnobMIDI instead of readKnob
    Serial.println("raw (unsmoothed) knob values:");
    Serial.println(b.readKnobRaw(1));
    Serial.println(b.readKnobRaw(2));
    Serial.println(b.readKnobRaw(3));
    Serial.println(b.readKnobRaw(4));
    
    delay(500);
  }
}

