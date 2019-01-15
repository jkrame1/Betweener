# Betweener
Learn to build and program the Betweener in the wiki: https://github.com/jkrame1/Betweener/wiki

![Betweener Panel](https://github.com/jkrame1/Betweener/blob/master/Hardware/Betweener%20Build%20Images/Betweener-75.jpg)

Description

The Betweener is an affordable, DIY module (Eurorack-compatible) for use in synthesis and multi-instrument systems.  It combines the flexibility of a Teensy 3.2 (programmable using the Arduino IDE) with four built-in Control Voltage (CV) inputs, four Trigger inputs, four knobs, and four CV outputs.  Via USB MIDI or an add-on DIN MIDI breakout, the Betweener enables MIDI-to-CV and CV-to-MIDI conversion in a highly customizable and hackable format.   

The Betweener is envisioned as a programmable platform that expands control capabilities for modular synthesis and MIDI-capable instrument systems and DAWs.  Typically it would not be used as an instrument on its own, but would sit “between” other devices, mediating control messages, converting signals from one format to another, or providing a central “brain” for a complex system.  However, because it is based around the Teensy (retaining the ability to use the associated Teensy Audio Shield), the possible applications are virtually limitless.  

For example, the Betweener can be used to:
- Translate MIDI CC signals (e.g. from a foot pedal) into CV outputs from 0 to 5 volts
- Use external triggers to trigger custom-programmed MIDI note sequences
- Take input from a variety of Teensy-compatible external sensors (e.g. motion sensors, light sensors, etc.) and convert      information into MIDI or CV outputs
- Use programmable knobs to modulate any CV-controlled or MIDI-controlled instrument
- Easily communicate between modular synthesizers and digital audio workstations such as Ableton Live and Reaper
