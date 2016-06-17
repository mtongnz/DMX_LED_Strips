/*

DMX_LED_Strips - Output Test
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

  
This sketch tests the RGB outputs.
It sets all red for 1 second, blue 1 second, green 1 second.  Finally white for 3 seconds and repeat.

Library Used:
    ShiftPWM by Elco Jacobs.    www.elcojacobs.com
 */

const int ShiftPWM_latchPin=8;
const bool ShiftPWM_invertOutputs = false;
const bool ShiftPWM_balanceLoad = true;

#include <ShiftPWM.h>

unsigned char maxBrightness = 255;
unsigned char pwmFrequency = 125;
unsigned int numRegisters = 2;
unsigned int numOutputs = numRegisters*8;

const int numRGB = 5;

void setup(){
  int x;
 
  // Setup PWM output
  ShiftPWM.SetAmountOfRegisters(numRegisters);
  ShiftPWM.Start(pwmFrequency,maxBrightness);
  
  // set all channels to 0 default
  ShiftPWM.SetAll(0);
}

void loop() {
  int x, y, z;
  
  ShiftPWM.SetAll(0);
  
  
  //delay(1000);
  
  for (y = 0; y < 3; y++) {
    for (x = 0; x <= 255; x++) {
      //ShiftPWM.SetAll(x);
      for (z = 0; z < numRGB; z++)
        ShiftPWM.SetOne((z*3) + y, x);
      delay(3);
    }
    
    for (x = 255; x >= 0; x--) {
      //ShiftPWM.SetAll(x);
      for (z = 0; z < numRGB; z++)
        ShiftPWM.SetOne((z*3) + y, x);
      delay(2);
    }
  }
  return;
  
  
  for (x = 0; x < 3; x++) {
    for (y = 0; y< numRGB; y++)
      ShiftPWM.SetOne((y*3) +x, 255);
    delay(1000);
    ShiftPWM.SetAll(0);
  }
  
  ShiftPWM.SetAll(255);
  delay(3000);
  
}
