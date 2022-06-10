#include <Arduino.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include "sleep.h"

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
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
  sleep_enable();
  sleep_mode();      // System sleeps here
  sleep_disable();   // System continues execution here when watchdog timed out
  sbi(ADCSRA, ADEN); // switch Analog to Digitalconverter ON
}
