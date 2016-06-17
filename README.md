# DMX_LED_Strips

Visit http://www.instructables.com/id/DMX-LED-Strips/ for instructions, schematics and video.

This sketch takes the DMX input and controls 5 RGB LED strips.

Outputs are handled by shift registers as the atMega328 doesn't have enough PWM outputs.  The number of channels can easily be increased by adding more shift registers.  For my purposes, 4 boxes with 5 LED strips each was the most effiecient for cable management.

Channel allocation is (1)red, (2)green, (3)blue, (4)strobe for each output (20 channels total).

Strobe:  0-19        off
         20 - 189    slow - fast
         190 - 205   slow random
         206 - 219   fast random
         220 - 255   off

Libraries Used:
  ShiftPWM by Elco Jacobs.    www.elcojacobs.com
  DMXSerial by Mat Hertel.    www.mathertel.de
