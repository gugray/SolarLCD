#include <Arduino.h>
#include "vcc.h"

volatile int16_t vcc = 0;

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

// For when playing with clock, maybe
// ADC Prescaler set to 64 - 125kHz@8MHz
// ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (0 << ADPS0);

int16_t measure_vcc_ext()
{
  // get_vcc_ext uses the internal reference 1.1V and measures voltage on a pin
  // Pin has a 1M resistor to battery voltage and 100k resistor to ground
  // Ie pin has 0.090909 times battery voltage, getting compared to 1.1V

  sbi(ADCSRA, ADEN);                          // switch Analog to Digitalconverter ON
  ADMUX = _BV(MUX1) | _BV(MUX0) | _BV(REFS1); // ADC3 on PB3, which is pin 3; against 1.1V internal reference
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA, ADSC))
    ;
  uint8_t low = ADCL;
  unsigned int val = (ADCH << 8) | low;
  // discard previous result
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA, ADSC))
    ;
  low = ADCL;
  val = (ADCH << 8) | low;

  ADCSRA = 0; // disable ADV

  // 41.3512 -> voltage
  // we return voltage * 100
  return ((long)1025 * val) / 878;
}
