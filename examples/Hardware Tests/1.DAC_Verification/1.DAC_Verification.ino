// DAC Verification Test
#include <SPI.h>


//One DAC is wired up to have chip select to Teensy digital pin 1
//the other to Teensy 2
int chip_select = 1;
int chip_select2 = 2;

//sets the chip select to high to start
//and starts communicating over normal SPI pins
//which are pins 13 (clock) and 11 (MOSI)
//unless we set those to the alternate options
// 14 (clock) and 7 (MOSI), which is what we
//want if we are using the audio shield too

void setup() {
  pinMode(chip_select, OUTPUT);
  digitalWrite(chip_select, HIGH); // HIGH);
  pinMode(chip_select2, OUTPUT);
  digitalWrite(chip_select2, HIGH); // HIGH);


  SPI.setMOSI(7);
  SPI.setSCK(14);
  SPI.begin();

  Serial.begin(115200);

}


void loop() {

  //ramping from 0 to maximum over 4096 steps
  //writing the output to both DACs on the chip at the
  //same time

  for (int i = 0; i < 4096; i ++) {
    MCP4922_write(chip_select, 0, i);  //OUTPUT 2
    MCP4922_write(chip_select, 1, i);  //OUTPUT 4
    MCP4922_write(chip_select2, 0, i); //OUTPUT 3
    MCP4922_write(chip_select2, 1, i); //OUTPUT 1
    delay(1);
  }
}

//this is the write function.  Pulls the chip select
//low in each write cycle and then sets it high again.
//the inscrutable stuff here is to get the right
//byte format for the chip
void MCP4922_write(int cs_pin, byte dac, int value) {
  byte low = value & 0xff;
  byte high = (value >> 8) & 0x0f;
  dac = (dac & 1) << 7;
  digitalWrite(cs_pin, LOW);
  SPI.transfer(dac | 0x30 | high);
  SPI.transfer(low);
  digitalWrite(cs_pin, HIGH);
}


