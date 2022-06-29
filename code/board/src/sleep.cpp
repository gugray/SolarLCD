#include <Arduino.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include "sleep.h"

volatile uint32_t msec = 0;

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

uint8_t lastSleepLength = 255;

void setupWatchdog(uint8_t sleepLength)
{
  if (sleepLength == lastSleepLength) return;
  else lastSleepLength = sleepLength;

  uint8_t b = sleepLength & 7;
  if (sleepLength > 7)
    b |= (1 << 5);
  b |= (1 << WDCE);
  MCUSR &= ~(1 << WDRF);

  // On Attiny, this is WDTCR
  // On Atmega (Nano), it's WDTCSR
#ifdef ENV_ATTINY
  // start timed sequence
  WDTCR |= (1 << WDCE) | (1 << WDE);
  // set new watchdog timeout value
  WDTCR = b;
  WDTCR |= _BV(WDIE);
#endif
#ifdef ENV_NANO
  // start timed sequence
  WDTCSR |= (1 << WDCE) | (1 << WDE);
  // set new watchdog timeout value
  WDTCSR = b;
  WDTCSR |= _BV(WDIE);
#endif
}

// Watchdog Interrupt Service / is executed when watchdog timed out
ISR(WDT_vect) {}

void sleep(uint8_t sleepLength)
{
  setupWatchdog(sleepLength);
  ADCSRA = 0; // disable ADV
  sleep_enable();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  
  // This *should* work, but it messes up the MCU
  // Investigate?
  // MCUCR = bit(BODS) | bit(BODSE);     // turn on brown-out enable select
  // MCUCR = bit(BODS);                  // this must be done within 4 clock cycles of above

  sleep_mode();
  sleep_disable(); // system continues execution here when watchdog timed out
  
  // Increment msec counter
  if (sleepLength == 0)
    msec += 16;
  else if (sleepLength == 1)
    msec += 32;
  else if (sleepLength == 2)
    msec += 64;
  else if (sleepLength == 3)
    msec += 128;
  else if (sleepLength == 4)
    msec += 250;
  else if (sleepLength == 5)
    msec += 500;
  else if (sleepLength == 6)
    msec += 1000;
  else if (sleepLength == 7)
    msec += 2000;
  else if (sleepLength == 8)
    msec += 4000;
  else if (sleepLength == 9)
    msec += 8000;
}
