#include <Arduino.h>
#include "vcc.h"

volatile int16_t vcc = 0;

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

void measureVCC()
{
  // get_vcc_ext uses the internal reference 1.1V and measures voltage on a pin
  // Pin has a 1M resistor to battery voltage and 100k resistor to ground
  // Ie pin has 0.090909 times battery voltage, getting compared to 1.1V

  sbi(ADCSRA, ADEN); // enable ADC

  ADCSRA |= 0b110;  // Prescaler: 64 -> 15kHz @1MHz
  ADMUX = _BV(MUX1) | _BV(MUX0) | _BV(REFS1); // ADC3 on PB3, which is pin 3; against 1.1V internal reference

  // Conversion takes 25 cycles when first enabled (13 afterwards)
  // Even at low ADC clock of 15kHz, that can no way be longer than 2msec
  // Count every 0.01msec = 10 micros up to 255; then, abandon
  uint8_t waitCount = 0;
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA, ADSC) && waitCount != 0xff)
  {
    delayMicroseconds(100);
    ++waitCount;
  }
  // Conversion didn't complete in time
  if (waitCount == 0xff)
    return;

  // Read value
  uint8_t low = ADCL;
  unsigned int val = (ADCH << 8) | low;

  ADCSRA = 0; // disable ADC

  // 41.3512 -> voltage
  // we return voltage * 100
  vcc = ((long)1025 * val) / 878;
}
