//Example sketch demonstrating a quad LFO for Betweener with codeable function-shape choices.
//Amplitude is controlled via CV in; master frequency is controlled by knob 1, with 
//other knobs controlling relative fractional frequencies.  Triggers reset phase.

//Note that there is some known glitch when you change frequency you can hear a wobble.
//this remains to be understood and debugged


#include <Betweener.h>

Betweener b;

////////////////////////
// set up the controls:

//frequency modulation will be done by modulating the period directly.  This
//range is thus 1/(max freq) to 1/(min freq) in units of seconds
float periodscalemin = 0.001;  
float periodscalemax = 10.;

// knob 1 will be a "master" control of the waveform period.
// other knobs will scale relative intervals, with the following 
// set of options.  If you modify these, be sure to update the 
// number tracking how many options there are, too.
//float period_ratios[] = {1./8., 1./4., 1./2., 1., 2., 4., 8.};
float period_ratios[] = {8., 3., 2., 1., 1./2., 1./3., 1./8.};
int n_ratios = 7;

// fixed phase offset variables for channels 2-4.  Units are percent of master cycle (e.g. 0.25 = 1/4 of a cycle)
// (these could be tied to a different form of control, in your own version of the code)
float phase_offset[] = {0.0, 0.0, 0.0};

//which waveform to use for each.  1 = sine; 2=square; 3=triangle; 4=ramp up/sawtooth; 5=ramp down/ sawtooth
byte waveform[] = {1, 4, 2, 5};


/////////////////////////
// define various constants 
// and declare variables

// for sine waves:
float twopi = 3.14159 * 2;

//stores the period we determine from each knob setting
float period; 
//stores the master period value, from knob 1
float master_period; 

 //stores amplitude read in from CV inputs
float ampl;

//a flag for restarting (or starting, the first time)
bool reset = true;

//set of microsecond timers, one for each LFO to keep things simple
elapsedMicros timer[4];

//fractional phase variable (takes on a value of 0 to 1 to indicate where we are in a cycle)
float fracphase;


//current scaling index
int scale;

//output value:
int val;

// counter to use for optional printout for debugging
int count=0;

///////////////////
// setup.  Serial.begin()
// is optional... it is there for debugging
void setup() {
  Serial.begin(115200);
  b.begin();

}


///////////////////////////
// main loop, executes at 
// whatever rate the chip can 
// handle

void loop() {

  // right now an overall reset is only triggered at the beginning
  // but this could be set to respond to some other input too
  if (reset) {
   for (int i = 0; i < 4; i++) {
      timer[i] = 0;

   } 
    reset = false;
  }

  //here we individually reset phase for each channel
  //based on triggers
  b.readTriggers();
  for (int i=0; i<4; i++){
    if (b.triggerRose(i+1)){
      timer[i] = 0;
    }
  }


  //loop through each of the 4 lfo/cv-out channels
  for (int j = 0; j < 4; j++) {

      //read CV input for the amplitude
      //default amplitude is full scale;
      //the number we read from the CV input attenuates.  
      // CV inputs are 10 bit, outputs are 12 bit.
      ampl = 4095 - map(b.readCV(j+1), 0, 1023, 0, 4095);

      //scale the (reversed) reading from knob 1 to a floating-point number between min and max period 
      //we reverse in order to have high freq. on the high end of the knob.
      master_period = ((1023.-float(b.readKnob(1)))) * ( (periodscalemax - periodscalemin)/1023. ) + periodscalemin;
      

      //chop up the full-scale range of the knob into n_ratios options
      //and figure out which to use
      scale = int(b.readKnob(j+1)/(float(ceil(1023./n_ratios))));    
      
      if (j == 0) {
          //master channel.
          period = master_period * 1e6;
            
      }else{      
          //scale the period by the appropriate ratio
          period=master_period * period_ratios[scale] * 1e6;
 
      }
      



    if (timer[j] < period) {
      //calculate where we are in the waveform cycle
      fracphase = timer[j] / period; // + phase_offset[j];

      //get the lfo function value for this phase value
      //the convertion here is to go from a scale of (-1, 1) to (0, 4095), which is the CV out range
      //and the range of the ampl variable

      val = int(((lfo(waveform[j], fracphase) + 1) * ampl) / 2.);  


/*
    //example diagnostic printout periodically for channel 4:
     if (j == 3){
         if (count < 500){
           count = count + 1;
         } else {
          count = 0;
          Serial.println(String("master period ")+master_period);
          Serial.println(String("this period ")+period);        
         }
      }
*/
      
      b.writeCVOut(j + 1, val); //j + 1 because the CV outs are number 1 through 4

    } else {

      timer[j] = 0;

    }

  }


}

float lfo(int func, float fractional_phase) {
  //correct the phase value if it exceeds one full cycle
  if (fractional_phase >= 1) {
    fractional_phase = fractional_phase - 1.;
  }


  switch (func) {
    case 1:
      // sine wave
      return sin(fractional_phase * twopi);

    case 2:
      //square wave
      if (fractional_phase < 0.5) {
        return 1.0;
      } else {
        return 0.0;
      }

    case 3:
      //triangle wave
      if (fractional_phase < 0.25) {
        return fractional_phase * 4.;
      } else if (fractional_phase >= 0.25 || fractional_phase < 0.75) {
        return 1 - (fractional_phase - 0.25) * 2;
      } else {
        return (fractional_phase - 0.75) * 4. - 1;
      }

    case 4:
      //ramp up
      return (fractional_phase * 2.) - 1.;

    case 5:
      //ramp down:
      return 1. - (fractional_phase * 2.);
    default:
      return 0.;
      break;
  }
}




