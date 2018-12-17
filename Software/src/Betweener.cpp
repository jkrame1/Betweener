//
//  Betweener.cpp
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
///////////////////////////////////////////////////////////////////////////////
//  Betweener.cpp detailed description:
//
//  This is the "implementation file" for the Betweener library.  Most of the
//  important overall comments about this library are in the associated
//  header file (Betweener.h).  This file is also obsessively commented, though,
//  with novice users in mind.
////////////////////////////////////////////////////////////////////////////////

//the functions and many variables in this file were all
//declared in Betweener.h, so we include it
#include "Betweener.h"

//Now, below, we have the code implementing all the functions
//(a.k.a. methods) of the Betweener class.
//The Betweener:: syntax specifies to the compiler that these
//functions live in the "Betweener namespace" which means that
//they will not conflict with other functions associated with
//other classes that have similar names.

Betweener::Betweener(void){
    //Constructor.  This code is run when you make a Betweener object.
    //We do not actually do much in this constructor.  For technical reasons
    //it is better to put all of the important stuff in a begin() function.
    
    //so... we just initialize some internal variables to nonsense values
    //so that they are not uninitialized.
    currentCV1=-1;
    currentCV2=-1;
    currentCV3=-1;
    currentCV4=-1;
    
    currentKnob1=-1;
    currentKnob2=-1;
    currentKnob3=-1;
    currentKnob4=-1;

    lastCV1=-1;
    lastCV2=-1;
    lastCV3=-1;
    lastCV4=-1;
    
    lastKnob1=-1;
    lastKnob2=-1;
    lastKnob3=-1;
    lastKnob4=-1;
    
}
    

void Betweener::begin(void){
    //this does all the real setup stuff
    
    //Note that al of the upper-case "variables" here are actually
    //defined in preprocessor directives in the .h file.
    //They cannot be modified at run time.
    
    //These are the settings needed to get the SPI working
    //for the DACs to create CV out
    pinMode(DAC_CHIP_SELECT1, OUTPUT);
    digitalWrite(DAC_CHIP_SELECT1, HIGH);
    pinMode(DAC_CHIP_SELECT2, OUTPUT);
    digitalWrite(DAC_CHIP_SELECT2, HIGH);
   
    //Start SPI
    //note that the order here is important when using the Audio shield
    SPI.setMOSI(SPI_MOSI);  //use alternate SPI MOSI
    SPI.setSCK(SPI_SCK);  //use alternate SPI SCK
    SPI.begin();
 
    
    //Here, we tell Teensy what to do with trigger pins
    pinMode(TRIGGER_INPUT1, INPUT_PULLUP);
    pinMode(TRIGGER_INPUT2, INPUT_PULLUP);
    pinMode(TRIGGER_INPUT3, INPUT_PULLUP);
    pinMode(TRIGGER_INPUT4, INPUT_PULLUP);
  
    
    // Create Bounce objects for each button.  The Bounce object
    // automatically deals with contact chatter or "bounce", and
    // it makes detecting changes very simple.
        // Note that bounce_ms is set to a default in the .h file
    trig1.attach(TRIGGER_INPUT1);
    trig1.interval(bounce_ms);
    
    trig2.attach(TRIGGER_INPUT2);
    trig2.interval(bounce_ms);
    
    trig3.attach(TRIGGER_INPUT3);
    trig3.interval(bounce_ms);
    
    trig4.attach(TRIGGER_INPUT4);
    trig4.interval(bounce_ms);
 

    //set up the smoothed analog readout objects
    //this depends on our own modified version of the
    //responsiveAnalogRead library
    smoothCV1.begin(CVIN1, RASleep, RASnapMultiplier);
    smoothCV1.setActivityThreshold(RAActivityThreshold);
    
    smoothCV2.begin(CVIN2, RASleep, RASnapMultiplier);
    smoothCV2.setActivityThreshold(RAActivityThreshold);
    
    smoothCV3.begin(CVIN3, RASleep, RASnapMultiplier);
    smoothCV3.setActivityThreshold(RAActivityThreshold);
    
    smoothCV4.begin(CVIN4, RASleep, RASnapMultiplier);
    smoothCV4.setActivityThreshold(RAActivityThreshold);
    
    smoothKnob1.begin(KNOB1, RASleep, RASnapMultiplier);
    smoothKnob1.setActivityThreshold(RAActivityThreshold);
  
    smoothKnob2.begin(KNOB2, RASleep, RASnapMultiplier);
    smoothKnob2.setActivityThreshold(RAActivityThreshold);
    
    smoothKnob3.begin(KNOB3, RASleep, RASnapMultiplier);
    smoothKnob3.setActivityThreshold(RAActivityThreshold);
    
    smoothKnob4.begin(KNOB4, RASleep, RASnapMultiplier);
    smoothKnob4.setActivityThreshold(RAActivityThreshold);
  
    
    //If we are using DIN MIDI I/O we need some setup:
#ifdef DODINMIDI
    Serial2.setRX(DINMIDIIN);
    Serial2.setTX(DINMIDIOUT);
    DINMIDI.begin(MIDI_CHANNEL_OMNI);  //monitor all input channels
#endif
    
}


void Betweener::readTriggers(void){
    //The bounce library has a function update() that is the
    //main read function
    trig1.update();
    trig2.update();
    trig3.update();
    trig4.update();
    
}


void Betweener::readCVs(void){
    //we could put stuff in here to limit the read
    //rate, but right now we'll leave that to the sketch
    
    lastCV1 = currentCV1;
    lastCV2 = currentCV2;
    lastCV3 = currentCV3;
    lastCV4 = currentCV4;
    
    smoothCV1.update();
    smoothCV2.update();
    smoothCV3.update();
    smoothCV4.update();
  
    currentCV1 = smoothCV1.getValue();
    currentCV2 = smoothCV2.getValue();
    currentCV3 = smoothCV3.getValue();
    currentCV4 = smoothCV4.getValue();
    

}


void Betweener::readKnobs(void){
 
    lastKnob1=currentKnob1;
    lastKnob2=currentKnob2;
    lastKnob3=currentKnob3;
    lastKnob4=currentKnob4;

    smoothKnob1.update();
    smoothKnob2.update();
    smoothKnob3.update();
    smoothKnob4.update();
    
    currentKnob1=smoothKnob1.getValue();
    currentKnob2=smoothKnob2.getValue();
    currentKnob3=smoothKnob3.getValue();
    currentKnob4=smoothKnob4.getValue();
    
}

void Betweener::readUsbMIDI(void) {
    //with each read, the usbMidi object
    //will store whatever messages it most recently received
    usbMIDI.read();
    
}


#ifdef DODINMIDI
void Betweener::readDINMIDI(void){
        DINMIDI.read();
}
#endif


void Betweener::readAllInputs(void){
    //run the previous four functions all in sequence
    readTriggers();
    readCVs();
    readKnobs();
    readUsbMIDI();
#ifdef DODINMIDI
    readDINMIDI();
#endif
}


int Betweener::readCV(int channel){
    int value = -1;
    
    switch (channel){
        case 1:
            lastCV1=currentCV1;
            smoothCV1.update();
            currentCV1 = smoothCV1.getValue();
            value = currentCV1;
            break;
            
        case 2:
            lastCV2=currentCV2;
            smoothCV2.update();
            currentCV2 = smoothCV2.getValue();
            value = currentCV2;
            break;
            
        case 3:
            lastCV3=currentCV3;
            smoothCV3.update();
            currentCV3=smoothCV3.getValue();
            value = currentCV3;
            break;
            
        case 4:
            lastCV4=currentCV4;
            smoothCV4.update();
            currentCV4=smoothCV4.getValue();
            value = currentCV4;
            break;
            
        default:
            DEBUG_PRINTLN("you are trying to read an nonexistent channel!");
            break;

    }
    return value;
}


int Betweener::readKnob(int channel){
    int value = -1;
    
    switch (channel){
        case 1:
            lastKnob1=currentKnob1;
            smoothKnob1.update();
            currentKnob1 = smoothKnob1.getValue();
            value = currentKnob1;
            break;
            
        case 2:
            lastKnob2=currentKnob2;
            smoothKnob2.update();
            currentKnob2 = smoothKnob2.getValue();
            value = currentKnob2;
            break;
            
        case 3:
            lastKnob3=currentKnob3;
            smoothKnob3.update();
            currentKnob3=smoothKnob3.getValue();
            value = currentKnob3;
            break;
            
        case 4:
            lastKnob4=currentKnob4;
            smoothKnob4.update();
            currentKnob4=smoothKnob4.getValue();
            value = currentKnob4;
            break;
            
        default:
            DEBUG_PRINTLN("you are trying to read an nonexistent channel!");
            break;
            
    }
    return value;
}



int Betweener::readKnobRaw(int channel){
    int value = -1;
    switch (channel){
        case 1:
            value = analogRead(KNOB1);
            break;
            
        case 2:
            value = analogRead(KNOB2);
            break;
            
        case 3:
            value = analogRead(KNOB3);
            break;
            
        case 4:
            value = analogRead(KNOB4);
            break;
        default:
            DEBUG_PRINTLN("you are trying to read an nonexistent channel!");
            break;
            
    }
    return value;
}


int Betweener::readCVRaw(int channel){
    int value = -1;
    
    switch (channel){
        case 1:
            value = analogRead(CVIN1);
            break;
            
        case 2:
            value = analogRead(CVIN2);
            break;
            
        case 3:
            value  = analogRead(CVIN3);
            break;
            
        case 4:
            value = analogRead(CVIN4);
            break;
            
        default:
            DEBUG_PRINTLN("you are trying to read an nonexistent channel!");
            break;
            
    }
    return value;
}



bool Betweener::knobChanged(int knob){
    //currently relying on the built-in change function in the
    //responsiveAnalogRead library.
    //note this function INITIATES A READ
 
    bool changed = false;
    switch(knob){
        case 1:
            smoothKnob1.update();
            changed=smoothKnob1.hasChanged();
            break;
        case 2:
            smoothKnob2.update();
            changed=smoothKnob2.hasChanged();
            break;
        case 3:
            smoothKnob3.update();
            changed=smoothKnob3.hasChanged();
            break;
        case 4:
            smoothKnob4.update();
            changed=smoothKnob4.hasChanged();
            break;
        default:
            DEBUG_PRINTLN("you are trying to read an nonexistent channel!");
            //not sure whether to return true or false as default...
            break;
            
    }
  
    
    return changed;
    
}


bool Betweener::CVChanged(int cv_channel){
    //like the knob case, this function INITIATES A READ
    //and checks to see if the value is a new one
    
    bool changed = false;

    switch(cv_channel){
        case 1:
            smoothCV1.update();
            changed = smoothCV1.hasChanged();
            break;
        case 2:
            smoothCV2.update();
            changed = smoothCV2.hasChanged();
            break;
        case 3:
            smoothCV3.update();
            changed = smoothCV3.hasChanged();
            break;
        case 4:
            smoothCV4.update();
            changed = smoothCV4.hasChanged();
            break;
        default:
            DEBUG_PRINTLN("you are trying to read an nonexistent channel!");
            break;
            
    }
    return changed;
}



int Betweener::readCVInputMIDI(int channel){
    return CVtoMIDI(readCV(channel));

}

int Betweener::readKnobMIDI(int channel){
    return knobToMIDI(readKnob(channel));
    
}


int Betweener::readKnobCV(int channel){
    return knobToCV(readKnob(channel));
}
    
int Betweener::CVtoMIDI(int val){
    //CV inputs are 10 bit (range 0-1023)
    //midi CC values go from 0 to 127, 7 bit
    //so we can get that by simply bit shifting
    int midival = val >>3;
    return midival;
    
}


int Betweener::MIDItoCV(int val){
    //midi CC values go from 0 to 127
    //CV outs are 12 bit (range 0-4095)
    int cvval = map(val, 0, 127, 0, 4095);
    return cvval;
    
}


int Betweener::knobToMIDI(int val){
    //knob inputs are 10 bit (range 0-1023)
    //midi CC values go from 0 to 127, 7 bit
    //so we can get that by simply bit shifting
    int midival = val >> 3;
    return midival;
    
}

int Betweener::knobToCV(int val){
    //knob inputs are 10 bit (range 0-1023)
    //CV outs are 12 bit (range 0-4095)
    int cvval = map(val,0,1023,0,4095);
    return cvval;
}



bool Betweener::triggerRose(int trigger){
    //note:  because of hardware setup, a rising trigger input is read as a
    //falling value at the Teensy pin, and vice versa
    //also note!  you must call readTriggers() first
    bool rose = false;
    switch(trigger){
        case 1:
            rose = trig1.fell();
            break;
        case 2:
            rose = trig2.fell();
            break;
        case 3:
            rose = trig3.fell();
            break;
        case 4:
            rose = trig4.fell();
            break;
        default:
            DEBUG_PRINTLN("you are trying to read an nonexistent trigger!");
            break;
            
    }
    return rose;
}


bool Betweener::triggerFell(int trigger){
    //note:  because of hardware setup, a rising trigger input is read as a
    //falling value at the Teensy pin, and vice versa
    //also note!  you must call readTriggers() first
    bool fell = false;
    switch(trigger){
        case 1:
            fell = trig1.rose();
            break;
        case 2:
            fell = trig2.rose();
            break;
        case 3:
            fell = trig3.rose();
            break;
        case 4:
            fell = trig4.rose();
            break;
        default:
            DEBUG_PRINTLN("you are trying to read an nonexistent trigger!");
            break;
            
    }
    return fell;
}


bool Betweener::triggerHigh(int trigger){
    //note:  because of hardware setup, a high input trigger
    //is read as a low at the Teensy pin and vice versa
    //also note!  you must call readTriggers() first
    int value = 0;

    switch(trigger){
        case 1:
            value = trig1.read();
            break;
        case 2:
            value = trig2.read();
            break;
        case 3:
            value = trig3.read();
            break;
        case 4:
            value = trig4.read();
            break;
        default:
            DEBUG_PRINTLN("you are trying to read an nonexistent trigger!");
            break;
            
    }
    //here's where the logic is reversed due to the high/low switch
    if (value == HIGH){
        return false;
    }else{
        return true;
    }

}


bool Betweener::triggerLow(int trigger){
    //note:  because of hardware setup, a high input trigger
    //is read as a low at the Teensy pin and vice versa
    //also note!  you must call readTriggers() first
    int value = 0;
    
    switch(trigger){
        case 1:
            value = trig1.read();
            break;
        case 2:
            value = trig2.read();
            break;
        case 3:
            value = trig3.read();
            break;
        case 4:
            value = trig4.read();
            break;
        default:
            DEBUG_PRINTLN("you are trying to read an nonexistent trigger!");
            break;
            
    }
    //here's where the logic is reversed due to the high/low switch
    if (value == LOW){
        return false;
    }else{
        return true;
    }
    
}


void Betweener::MCP4922_write(int cs_pin, byte dac, int value){
    // Adapted from code by Sebastian Tomczak
    // from a tutorial here:  http://little-scale.blogspot.com/2016/11/teensy-and-mcp4922-dual-channel-12-bit.html

    byte low = value & 0xff;
    byte high = (value >> 8) & 0x0f;
    dac = (dac & 1) << 7;
    digitalWrite(cs_pin, LOW);
    //Using beginTransaction and endTransaction to allow for the use of audio shield at the
    //same time.  The settings here are for SPI communication with the chip, which
    //works with the default mode 0 and with byte order MSB first.  I am unsure of the
    //best clock speed choice so this might be something to tweak if it becomes buggy
    SPI.beginTransaction(SPISettings(4000000,MSBFIRST,SPI_MODE0));
    SPI.transfer(dac | 0x30 | high);
    SPI.transfer(low);
    SPI.endTransaction();
    digitalWrite(cs_pin, HIGH);
}


void Betweener::writeCVOut(int cvout, int value){
    int cs_pin = -1;
    byte dac = -1;
    switch (cvout){
        case 1:
            cs_pin = CVOUT1_CHIP_SELECT;
            dac = CVOUT1_DAC_CHANNEL;
            break;
        case 2:
            cs_pin = CVOUT2_CHIP_SELECT;
            dac = CVOUT2_DAC_CHANNEL;
            break;
        case 3:
            cs_pin = CVOUT3_CHIP_SELECT;
            dac = CVOUT3_DAC_CHANNEL;
            break;
        case 4:
            cs_pin = CVOUT4_CHIP_SELECT;
            dac=CVOUT4_DAC_CHANNEL;
            break;
        default:
            DEBUG_PRINTLN("you are trying to write to a nonexistent CV channel!");
            break;
       
    }
    
    //should put in some idiot checks here that the value is reasonable...
    
    MCP4922_write(cs_pin, dac,value);
}

