#include <stdlib.h>
#include <avr/io.h>     // this contains all the IO port definitions
#include <avr/wdt.h>    // Watchdog
#include <avr/sleep.h>  // Sleep Mode
#include <avr/interrupt.h> // cli() etc.

inline void status(uint8_t b) {
  DDRB = 0xff;
  PORTB = ~b; /* common positive */
}

#ifndef PCIE0
#define PCIE0 PCIE
#endif
#ifndef PCIE2
#define PCIE2 4
#endif

enum {
  state_start,
  /* In Setup mode, we let the user train us by flashing the light on and off at 0.25Hz.
   * User must put finger when on, and take it back when off.
   * During the first 0.5s of each 2s "on" period we do not collect data (this gives the user
   * time to actually move their finger). We then sample for 1.5s.
   */
  state_setup_led_off = 0x9,
  state_setup_led_off_waiting, /* 0.5s LED off, no data gathering */
  state_setup_led_off_start,
  state_setup_led_off_low,
  state_setup_led_off_charging,

  state_setup_led_on = 0x11,
  state_setup_led_on_waiting, /* 0.5s LED on, no data gathering ("move finger" period)*/
  state_setup_led_on_start,
  state_setup_led_on_low,     /* 0.03s LED on, input low = discharge */
  state_setup_led_on_charging,     /* LED on, input Hi-Z = charge */

  /* Depending on the data gathered so far, we either
   * - go back to state_setup_led_off (try again)
   * - or have enough data to move to work state.
   *
   * In work state we sample at 20Hz and do something useful if we detect the finger
   * is present.
   */
  state_work = 0x21,
  state_work_start,        /* 0.01s Input low = discharge */
  state_work_low,          /* input Hi-Z = charge */
  state_work_charging
}; uint8_t state;

/* Keep these two in sync */
#define intervals(t) (t/0.030)
#define TIMEBASE WDTO_30MS

inline void watchdog_init() {
  /* Enable watchdog */
  wdt_reset();
  /* Watchdog will trigger ~ every 30ms
   * (based on the internal 128KHz clock). */
  wdt_enable(TIMEBASE);

  #ifdef WDTCSR  /* 2313 */
  /* Set WDCE (change enable), Set WDIE (interrupt) */
  WDTCSR |= _BV(WDCE) | _BV(WDIE);
  /* Clear WDE (no reset) */
  WDTCSR &= ~(_BV(WDE));
  #endif

  #ifdef WDTCR   /* 2313A */
  /* Set WDCE (change enable), Set WDIE (interrupt) */
  WDTCR |= _BV(WDCE) | _BV(WDIE);
  /* Clear WDE (no reset) */
  WDTCR &= ~(_BV(WDE));
  #endif

  /* Reduce power, but still wake up on Watchdog, .. */
  // set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  set_sleep_mode(SLEEP_MODE_IDLE);
}

inline void power_init(void) {
  /* Disable BOD mode */
  /* Done by fuses */

  /* Disable the Analog Comparator */
  ACSR = _BV(ACD);

  /* Turn off Counter0, and USART */
  PRR = _BV(PRTIM0) | _BV(PRUSART);
}

/* INPUT is PD6/ICPI , pin #11 on ATTiny2313A PDIP/SOIC */
#define INPUT_PORT PORTD6

/* Connect a pull-up resistor between Vcc and the input port.
 * Tested values for pull-up: 100K-ohm, 1M-ohm.
 */

inline void ports_init(void) {
  /* Configure PORTD */
  PORTD &= ~_BV(INPUT_PORT); /* PD6 is kept output-low/input-Hi-Z */
  /* Note: this should already be the default. */
}

inline void set_input_low(void) {
  /* Set to output (low) */
  DDRD |= _BV(INPUT_PORT);
}

inline void set_input_charge(void) {
  /* Set input Hi-Z */
  DDRD &= ~_BV(INPUT_PORT);
}

inline void ioinit(void) {
  /* Prevent any interrupt while we are changing things */
  cli();

  /* Initialize power-saving modes */
  power_init();

  ports_init();
  /* Hardware initialization is done. Proceed with data initialization. */

  /* Indicate the state to the watchdog handler */
  state = state_start;

  watchdog_init();

  /* Let's get started (re-establish interrupts) */
  sei();
}

/* Use port D0 (pin #2 on ATTiny2313A) as output. */
#define LED_PIN PORTD0

inline void led_off(void) {
  DDRD |= _BV(LED_PIN);
  PORTD &= ~_BV(LED_PIN);
}
inline void led_on(void) {
  DDRD |= _BV(LED_PIN);
  PORTD |= _BV(LED_PIN);
}

inline void start_measure(void) {
  /* Activate noise canceler, detection on rising edge */
  /* clkIO with /256 prescaling. */
  TCCR1B |= _BV(ICNC1) | _BV(ICES1) | _BV(CS10);

  /* Clear Input Capture Flag */
  TIFR |= _BV(ICF1); /* "ICF1 can be cleared by writing a logic one to its bit location" */

  /* Clear the counter */
  TCNT1 = 0;
}

inline uint8_t measured(void) {
  return TIFR & _BV(ICF1);
}

/* Watchdog timer handler */

uint8_t counter = 0;

inline void to(uint8_t new_state) {
  state = new_state;
  counter = 0;
}

uint8_t measures = 0;

uint16_t led_off_max = 0;
uint16_t led_on_min = 0xffff;

const uint8_t wait_timeout = intervals(1.500);
const uint8_t discharge_cycles = 2;
const uint8_t max_measures = intervals(2.500);

ISR( WDT_OVERFLOW_vect, ISR_BLOCK ) {
  counter++;
  status(state);

  switch(state) {
    case state_start:
      led_off_max = 0;
      led_on_min = 0xffff;
      led_on(); /* blink to indicate start */
      to(state_setup_led_off);
      return;

    case state_setup_led_off:
      measures = max_measures;
      led_off();
      to(state_setup_led_off_waiting);
      return;

    case state_setup_led_off_waiting:
      if(counter >= wait_timeout) {
        to(state_setup_led_off_start);
      }
      return;

    case state_setup_led_off_start:
      if(counter < discharge_cycles) {
        set_input_low();
      } else {
        to(state_setup_led_off_low);
      }
      return;

    case state_setup_led_off_low:
      start_measure();
      set_input_charge();
      to(state_setup_led_off_charging);
      return;

    case state_setup_led_off_charging:
      if(measured()) {
        uint16_t measure = ICR1;
        // status(measure>>8);
        if(measure > led_off_max) {
          led_off_max = measure;
        }
        measures--;
        if(measures) {
          to(state_setup_led_off_start);
          return;
        }
        /* Done measuring here */
        to(state_setup_led_on);
      }
      return;

    case state_setup_led_on:
      measures = max_measures;
      led_on();
      to(state_setup_led_on_waiting);
      return;

    case state_setup_led_on_waiting:
      if(counter >= wait_timeout) {
        to(state_setup_led_on_start);
      }
      return;

    case state_setup_led_on_start:
      if(counter < discharge_cycles) {
        set_input_low();
      } else {
        to(state_setup_led_on_low);
      }
      return;

    case state_setup_led_on_low:
      start_measure();
      set_input_charge();
      to(state_setup_led_on_charging);
      return;

    case state_setup_led_on_charging:
      if(measured()) {
        uint16_t measure = ICR1;
        // status(measure>>8);
        if(measure < led_on_min) {
          led_on_min = measure;
        }
        measures--;
        if(measures) {
          to(state_setup_led_on_start);
          return;
        }
        /* Done measuring: Decision time! */
        if(led_off_max < led_on_min) {
          /* Looks good, switch to normal mode. */
          to(state_work);
        } else {
          /* Setup failed, start over. */
          to(state_start);
        }
      }
      return;

    case state_work:
      led_off();
      to(state_work_start);
      return;

    case state_work_start:
      if(counter < discharge_cycles) {
        set_input_low();
      } else {
        to(state_work_low);
      }
      return;

    case state_work_low:
      start_measure();
      set_input_charge();
      to(state_work_charging);
      return;

    case state_work_charging:
      if(measured()) {
        uint16_t measure = ICR1;
        if(measure <= led_off_max) {
          led_off();
        }
        if(measure >= led_on_min) {
          led_on();
        }
        to(state_work_start);
      }
      return;

  }
}

/* Startup */
__attribute__((naked))    // suppress redundant SP initialization
extern int main(void) {
  ioinit();

  while(1) {
    sleep_mode();
  }

  return 0;
}

/* Build Chain Breakage on my machine */
void exit(int __status) {}
