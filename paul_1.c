#include <avr/io.h>      // this contains all the IO port definitions
#include <util/delay.h>

// This function basically wastes time
void delay_ms( uint16_t milliseconds)
{
   for( ; milliseconds > 0; milliseconds--)
   {
      _delay_ms( 1);
   }
}


int main(void) {

  uint16_t c = 0;

  int on = 0;

  DDRB = 0xFF;       // set all 8 pins on port B to outputs
  PORTB = 0x0;       // set all pins on port B low (turn off LEDs)

  // Port D0 en Hi-Z/Output Low
  // Autre ports Dn en Pulled-Up/Output High
  PORTD = 0xfe;

  // Turn pull-ups On
  MCUCR &= ~(1<<PUD);

  uint16_t d = 0;
  int led = 0;

  uint16_t max = 100;
  uint16_t wait  = 0;

  while(1) {
    if(PIND & 1) {
      if(c > max) { // 100 works with dry fingers.
        on = 1;
      } else {
        on = 0;
      }
      // Port D0 en output 0, others Dn in input
      DDRD = 0x01;
      // Port D0 in input Hi-Z, others in input
      DDRD = 0x00;
      c = 0;
    } else {
      c++;
    }
    PORTB = (max<<1)+on;

    wait++;
    if(wait > 10000) {
      if(PIND & 2) {
        max++;
      }
      if(PIND & 4) {
        if(max > 0){
          max--;
        }
      }
      wait = 0;
    }
/*
    if(on) {
      d += 2;
    } else {
      d += 1;
    }
    if(d > 40) {
      led = !led;
      d = 0;
    }
    if(led) {
        PORTB = 0xff;
    } else {
        PORTB = 0x00;
    } // on
*/
  }
}
