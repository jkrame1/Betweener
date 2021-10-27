//
//  Betweener.h
//
//  Copyright (c) 2018 Kathryn Schaffer
 
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in all
//  copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//  SOFTWARE.
//
///////////////////////////////////////////////////////////////////////////
//  Betweener.h detailed description:
//  
//  This .h file defines the Betweener class, setting up all the
//  variables and providing the "function prototypes" that tell the
//  compiler what functions the class contains.  The associated .cpp file has
//  the implementation (the actual code) for all the functions.
//
//  The purpose of the Betweener class is to set up the software mappings
//  that enable Teensy to communicate via the soldered-together connections
//  on the Betweener device itself.
//
//  The Betweener class also includes functions that allow a
//  slightly streamlined set of read/write operations so that the
//  device can mediate between CV inputs and outputs, MIDI inputs and
//  outputs, and trigger inputs.
//
//  The class has been written assuming it will be read and modified
//  by users with little coding experience, and is thus obsessively
//  commented with that audience in mind.
//
//  To test the Betweener hardware and see examples of how to use this
//  library, load and run the sketch betweener_hardware_tests.ino
//
//  ADDITIONAL CREDITS:
//      Betweener hardware design: Joseph Kramer
//      This code uses code snippets from:
//          Sebastian Tomczak (MCP4922 write function)
//
//////////////////////////////////////////////////////////////////////////

//these statements are a standard C++ way to avoid accidentally
//re-loading this .h file multiple times, which can make for weird
//conflicts in parameter and function definitions in certain cases
#ifndef Betweener_h
#define Betweener_h


//Here is where we include all the other Arduino libraries that this
//depends on.  We're using the updated "Bounce2" library because it
//is more compatible with being used in other libraries
#include <Arduino.h>
#include <SPI.h>
#include <Bounce2.h>
#include <MIDI.h>
#include <ResponsiveAnalogRead.h>


//This is where we define hard-wired pin associations.
//These are "preprocessor directive" statements (with # symbol) instead of
//variable definitions.  We use uppercase names in order to make it
//easy to tell that these are not normal variables when we invoke them in the
//code.  The difference is that #define statements are only implemented when
//the code is compiled, and never when the code is just being run.
//That makes it impossible for any of the code to actively change these
//parameters while it is running, which is a good idea since the
//definitions relate to hard-wired configuration choices.

//SPI is a communication protocol used for the digital-analog-converters (DACs)
//in the device.  These are used to create the CV outputs.
//SPI communication requires two pins for special functions,
//called MOSI and SCK.
#define SPI_MOSI 7 // note this is technically the "alternate" MOSI option for Teensy
#define SPI_SCK 14
//Also for the SPI protocol, these parameters are used to select
//which DAC we are sending messages to
#define DAC_CHIP_SELECT1 1
#define DAC_CHIP_SELECT2 2
//Identify the 4 CV outs with the right chip and DAC.
//This is something that may change for different board versions.
#define CVOUT1_CHIP_SELECT 2
#define CVOUT1_DAC_CHANNEL 1
#define CVOUT2_CHIP_SELECT 1
#define CVOUT2_DAC_CHANNEL 0
#define CVOUT3_CHIP_SELECT 2
#define CVOUT3_DAC_CHANNEL 0
#define CVOUT4_CHIP_SELECT 1
#define CVOUT4_DAC_CHANNEL 1

//Other pins used for inputs (which may change for different board versions)
//digital pins used for trigger inputs
#define TRIGGER_INPUT1 0
#define TRIGGER_INPUT2 3
#define TRIGGER_INPUT3 5
#define TRIGGER_INPUT4 4
//analog inputs used for the 4 CV inputs
#define CVIN1 A7
#define CVIN2 A6
#define CVIN3 A3
#define CVIN4 A2
//analog inputs used for the 4 potentiometers/knobs
#define KNOB1 A12
#define KNOB2 A13
#define KNOB3 A11
#define KNOB4 A10


//use this flag to decide whether to make use of DIN midi or not
//If you want it, uncomment the line in the code that says #define DODINMIDI.
//Otherwise, leave it commented it out.
//Note that if you use MIDI DIN, you will need
//the updated MIDI library for Teensy, from here:
//https://github.com/FortySevenEffects/arduino_midi_library
//and you will need to delete the default MIDI.h file that
//is included in the Arduino package.
//#define DODINMIDI


//If we do choose to use hardware MIDI, these will be
//the pins.  Alternate pins for Serial2.  See:
//https://www.pjrc.com/teensy/td_uart.html
#define DINMIDIIN 26
#define DINMIDIOUT 31


//If you want the code to give you some error messages in the
//serial monitor, leave the line below. Otherwise, comment it out.
#define DEBUG  //leave this line in to have automatic error printouts to Serial monitor
//this little "preprocessor macro" (which is like a function but processed at a different time)
//will be used to print the debug info.  See examples in the .cpp file for how this gets used.
#ifdef DEBUG
#define DEBUG_PRINT(...) Serial.print(__VA_ARGS__)
#define DEBUG_PRINTLN(...) Serial.println(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#define DEBUG_PRINTLN(...)
#endif


//////////////////////////////////////////////////////////////////////////////
//Now we define the Betweener class, which will let us make Betweener objects
//in our sketches.  The class organizes a set of functions (called "methods") and
//the set of variables that they use and control.  A special method called the
//"constructor" is the one that initially builds the object and puts in all the
//default variable definitions.  The other methods provide a flexible interface
//for defining how the Betweener maps inputs to outputs and for controlling
//its read/write operations and other operations.
//
//All variables and all methods get declared in this .h file, but the actual
//code for the functions themselves lives in the .cpp file, not here.  The
//.h file does have some core definitions of defaults for some variables, but
//mostly it exists to tell the compiler how much space to allocate when
//building a Betweener object, and what kinds of structures to expect the
//Betweener objects to contain.  The compiler then looks in the .cpp file to
//fill in the details of the actual functions/code.

class Betweener
{
    /////////////////////////////////////////////////////////////////////////////
    //The "public" parts of the Betweener class are the ways a user will interact
    //with it.  This includes the "constructor" function (which has the same name
    //as the class itself) that makes and initializes individual Betweener objects
    //in user code. It also includes the functions you will use to make the
    //Betweener do what you want with reading, writing, etc.
    
    public:
    
    Betweener();  //constructor; initializes some Betweener-specific variables
    void begin(void); //starts up all the other objects this depends on
    
    
    ////////////////////////
    // INPUT FUNCTIONS
    // these are the functions for making the Betweener read inputs.
    // you have two choices for how to handle inputs...
    // you can ask Betweener to read everything and just store the
    // numbers, which you then retrieve and deal with yourself,
    // or you can ask it to perform a read and give you back a number
    // for an individual channel.

    //these read functions load up current values of all inputs into
    //the object.  Access those by accessing member variables, e.g.
    // b.curentCV1
    void readTriggers(void);  //reads all triggers
    void readCVs(void); //reads analog inputs (CV inputs)
    void readKnobs(void);  //reads potentiometer inputs
    void readUsbMIDI(void);  //reads MIDI via the usbMIDI arduino functionality
#ifdef DODINMIDI
    void readDINMIDI(void);  //reads MIDI via the 5-pin DIN connector
#endif
    
    void readAllInputs(void); //reads triggers, CV, knobs, and USB MIDI inputs, in that order
    
    //these functions read individual channels and return the
    //values directly:
    int readCV(int channel);   //returns a smoothed 10 bit number
    int readKnob(int channel);     //returns a smoothed 10 bit number
    
    int readCVRaw(int channel);  //returns an un-smoothed 10-bit number
    int readKnobRaw(int channel);  //returns an un-smoothed 10-bit number

    //these versions do some converting for you
    int readCVInputMIDI(int channel);
    int readKnobMIDI(int channel);
    int readKnobCV(int channel);
    
    //functions that check for change.  Note these functions
    //INITIATE A READ and check for change.
    //they also use the algorithm packaged with the smoothing algorithm.
    bool knobChanged(int knob);
    bool CVChanged(int cv_channel);
    
    //functions that provide a convenient wrapping for Bounce2 functions.
    //note that because of the hardware implementation, a high trigger input
    //ends up being a low when it hits the chip, thus a rising trigger
    //is actually read in as a falling trigger.  These functions do the right thing
    //so you don't have to keep track of it.
    //NOTE:  you must call readTriggers before these functions!
    bool triggerRose(int trigger);
    bool triggerFell(int trigger);
    bool triggerHigh(int trigger);
    bool triggerLow(int trigger);

    
    
    /////////////////////////
    //BASIC OUTPUT FUNCTIONS
    //These functions assume you are using your sketch to decide directly what
    //output to write.  The functions are mainly useful for just hiding some of the
    //messier logic required by the specific DAC chip, etc.
    void writeCVOut(int cvout, int value); //cvout selects channel 1 through 4; value is in range 0-4095
    
    //these are setup functions you can call to override parameter defaults before calling 'begin'
    //so that nothing needs to be recompiled to try different options.
    //the default options are hard-coded down below in this .h file
    void setBounceMillisec(int millisec){bounce_ms = millisec;};
    void setRASnapMultiplier(float snap){RASnapMultiplier = snap;};
    void setRAActivityThreshold(int thresh){RAActivityThreshold=thresh;};
    void setRASleep(bool sleep){RASleep = sleep;};
    
    //static means that these functions can be accessed without necessarily having
    //a "Betweener" object available.  Just by including this library/class,
    //they can be accessed via Betweener::MCP4922_write etc.
    static void MCP4922_write(int cs_pin, byte dac, int value);
    
    //Scaling and conversion functions.  You can use these directly
    //and they are also used by some of the read functions
    int CVtoMIDI(int val);
    int MIDItoCV(int val);
    int knobToMIDI(int val);
    int knobToCV(int val);

    // these are the variables we use to store current analog input readings,
    // updated whenever a read operation is performed
    int currentCV1;
    int currentCV2;
    int currentCV3;
    int currentCV4;

    // these are the variables that store current knob / pot readings,
    // updated whenever a read operation is performed
    int currentKnob1;
    int currentKnob2;
    int currentKnob3;
    int currentKnob4;

    // these are the variables we use to store previous values
    // updated whenever a read operation is performed
    int lastCV1;
    int lastCV2;
    int lastCV3;
    int lastCV4;
    
    // these are the variables that store previous knob / pot readings,
    // updated whenever a read operation is performed
    int lastKnob1;
    int lastKnob2;
    int lastKnob3;
    int lastKnob4;
    
    // these are the trigger inputs.  We use the Bounce library, which provides
    // a way to avoid accidental triggers due to fluctuating inputs
    Bounce trig1;
    Bounce trig2;
    Bounce trig3;
    Bounce trig4;
    
    
    //midi interface.  Don't freak out about how weird this looks.  Look up "c++ templates" for more info.
    //Note that we are going to remap the Serial2 pins and using those for DIN MIDI IO.
    //Also we only do this if we are going to compile the hardware MIDI stuff
#ifdef DODINMIDI
    midi::MidiInterface<HardwareSerial> DINMIDI = midi::MidiInterface<HardwareSerial>((HardwareSerial&)Serial2);
#endif

    
    //the "private" parts of the Betweener class are the interior guts - the ways
    //that the class manages some of the functionality of the device internally.
    //We also have some private variables, which are variables that functions in the
    //class can modify, but a user cannot directly touch... only via a "set" or "get"
    //function.

    
    //right now this class is written so that almost everything is in the hands
    //of the user.. except we want some parameters to be a little harder to modify by
    //accident, so those go here.  Also we hide the smoothing-analog-read objects here.
    private:

    int bounce_ms = 5;
    float RASnapMultiplier = 0.015; //snapMultiplier parameter for the ResponsiveAnalogRead library
    int RAActivityThreshold = 10; //activity threshold parameter for ResponsiveAnalogRead
    bool RASleep = true;  //sleep parameter for ResponsiveAnalogRead
    
    ResponsiveAnalogRead smoothKnob1;
    ResponsiveAnalogRead smoothKnob2;
    ResponsiveAnalogRead smoothKnob3;
    ResponsiveAnalogRead smoothKnob4;
    
    ResponsiveAnalogRead smoothCV1;
    ResponsiveAnalogRead smoothCV2;
    ResponsiveAnalogRead smoothCV3;
    ResponsiveAnalogRead smoothCV4;
  
    
};



#endif /* Betweener_h */
