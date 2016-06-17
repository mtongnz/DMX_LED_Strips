/*

DMX_LED_Strips
Copyright (c) 2015, Matthew Tong
https://github.com/mtongnz/DMX_LED_Strips
http://www.instructables.com/id/DMX-LED-Strips/

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.
If not, see http://www.gnu.org/licenses/

  
This sketch takes the DMX input and controls 5 RGB LED strips.

Channel allocation is red, green, blue, strobe for each output (20 channels total).

Strobe:  0-19        off
         20 - 189    slow - fast
         190 - 205   slow random
         206 - 219   fast random
         220 - 255   off

Libraries Used:
  ShiftPWM by Elco Jacobs.    www.elcojacobs.com
  DMXSerial by Mat Hertel.    www.mathertel.de
*/

// ShiftPWM settings
const int ShiftPWM_latchPin=8;
const bool ShiftPWM_invertOutputs = false;
const bool ShiftPWM_balanceLoad = true;

#include <DMXSerial.h>
#include <ShiftPWM.h>

// ShiftPWM settings
unsigned char maxBrightness = 255;
unsigned char pwmFrequency = 125;
unsigned int numRegisters = 2;
unsigned int numOutputs = numRegisters*8;

// Status LED pin
const int statusLEDPin = 5;

// Output settings
const int numRGBS = 5;
const int dmxNumChan = numRGBS * 4;

// DMX address.
const int dmxStartAddr = 1;  // unit 1 & 2 = 61, unit 3 & 4 = 81

// Counter variables
int strobeCount[numRGBS];
int randStrobeCount[numRGBS];
int timerCount;

void setup(){
  int x;

  // Random Seed
  randomSeed(analogRead(0));
  
  // Setup DMX receiver
  DMXSerial.init(DMXReceiver);

  // Setup PWM output
  ShiftPWM.SetAmountOfRegisters(numRegisters);
  ShiftPWM.Start(pwmFrequency,maxBrightness);
  
  // set all channels to 0 default
  ShiftPWM.SetAll(0);
  
  // Setup status LED
  pinMode(statusLEDPin, OUTPUT);
  digitalWrite(statusLEDPin, 1);
  
  // Set counters to zero
  for (x = 0; x < numRGBS; x++) {
    strobeCount[x] = 0;
    randStrobeCount[x] = random(5, 200);
  }
  timerCount = 0;

  //set timer0 interrupt at 200Hz
  cli();                                // disable interupts
  TCCR0A = 0;                           // set entire TCCR0A register to 0
  TCCR0B = 0;                           // same for TCCR0B
  TCNT0  = 0;                           // initialize counter value to 0
  OCR0A = 78;                           // set compare match register for 200hz increments
  TCCR0A |= (1 << WGM01);               // turn on CTC mode
  TCCR0B |= (1 << CS02) | (1 << CS00);  // Set CS02 and CS00 bits for 1024 prescaler
  TIMSK0 |= (1 << OCIE0A);              // enable timer compare interrupt
  sei();                                // enable interupts
}

// Timer0 interupt handler.  Increments all the counters.
ISR(TIMER0_COMPA_vect){
  int x;
  
  for (x = 0; x < numRGBS; x++)
    strobeCount[x]++;
  
  timerCount++;
}

void loop() {
  int x, y, s;
  
  if (timerCount >= 100) {
    // Blink status LED if DMX receiving
    if (DMXSerial.noDataSince() < 1000) {
      digitalWrite(statusLEDPin, digitalRead(statusLEDPin) ^ 1);
      
    //   Turn status LED red if no DMX received
    }else
      digitalWrite(statusLEDPin, 1);
    timerCount = 0;
  }
  
  // Turn off all outputs if we haven't received DMX in 10 seconds
  if (DMXSerial.noDataSince() > 10000)
    ShiftPWM.SetAll(0);
  else {
    // loop through each of the 'y' RGB leds
    for (y = 0; y < numRGBS; y++) {
      s = DMXSerial.read(dmxStartAddr + (y * 4));
      
      //  strobe mode
      if (s > 20 && s <= 220) {
        int t;
        
        // random strobe
        if (s > 190) {
          t = (40 - (s - 190)) * 10;
          s = randStrobeCount[y];
        }

        
        // strobe on
        if (strobeCount[y] >= ( (192 - s) + 5) && strobeCount[y] < 500) {
          for (x = 0; x < 3; x++)
            ShiftPWM.SetOne((y*3) + x, DMXSerial.read(dmxStartAddr + (y * 4) + x + 1));
          strobeCount[y] = 500;
          TCNT0  = 0;  //clear timer
        }
        
        // strobe off
        if (strobeCount[y] >= 501 ) {
          for (x = 0; x < 3; x++)
            ShiftPWM.SetOne((y*3) + x, 0);
          strobeCount[y] = 0;
          
          randStrobeCount[y] = random(1, t);
        }
        
        
      // non strobe mode
      } else {
        strobeCount[y] = 0;
        
        // Get DMX values and set outputs
        for (x = 0; x < 3; x++)
          ShiftPWM.SetOne((y*3) + x, DMXSerial.read(dmxStartAddr + (y * 4) + x + 1));
      }
    }
  }
}
