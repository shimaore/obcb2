Build
=====

Solder 1M-ohm resistor between pins 20 (Vcc) and 11 (port D6).
Solder LED with short lead on port 10 (GND) and long lead on port 2 (PD0).
Solder battery plus to Vcc (20) and minus to GND (10).
Port 11 is the "touch" port.

Usage
=====

On first startup (or if not already configured) the LED will flash then go off for a few seconds, then go back on for a few seconds, etc.

The goal is to program the circuit by holding the touch lead when the LED is on, and leave the touch lead alone when the LED is off.

Once programmed the LED will flash faster at startup; touching the "touch" lead will make it run even faster then with rate decay.

Pinout (ATtiny2313A)
====================

01
02 output (flashes faster when touch is active)
03 "reset eeprom" (connect to Vcc at startup to reset calibration)
04
05
06
07
08
09
10 GND
11 "Touch"
12 B0 (clock 1/2)
13 B1 (clock 1/4)
14 B2 (clock 1/8)
15 B3 (message: bottom row)
16 B4 (message)
17 B5 (message)
18 B6 (message)
19 B7 (message: top row)
20 Vcc (+3V)
