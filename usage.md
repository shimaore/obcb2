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
