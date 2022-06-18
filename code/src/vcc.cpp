#include <Arduino.h>
#include "vcc.h"

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

// For when playing with clock, maybe
// ADC Prescalar set to 64 - 125kHz@8MHz
// ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (0 << ADPS0);


int16_t get_vcc()
{
    // get_vcc uses VCC as the reference and measures the VBG, the bandgap reference voltage
    // VBG is nominally 1.1V

    sbi(ADCSRA, ADEN); // switch Analog to Digitalconverter ON

//reads internal 1V1 reference against VCC
#if defined(__AVR_ATtiny84__) || defined(__AVR_ATtiny44__)
    ADMUX = _BV(MUX5) | _BV(MUX0); // For ATtiny84
#elif defined(__AVR_ATtiny85__) || defined(__AVR_ATtiny45__)
    ADMUX = _BV(MUX3) | _BV(MUX2); // For ATtiny85/45
#elif defined(__AVR_ATmega1284P__)
    ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1); // For ATmega1284
#else
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1); // For ATmega328
#endif
    delay(2);            // Wait for Vref to settle
    ADCSRA |= _BV(ADSC); // Convert
    while (bit_is_set(ADCSRA, ADSC))
        ;
    uint8_t low = ADCL;
    unsigned int val = (ADCH << 8) | low;
    //discard previous result
    ADCSRA |= _BV(ADSC); // Convert
    while (bit_is_set(ADCSRA, ADSC))
        ;
    low = ADCL;
    val = (ADCH << 8) | low;

    ADCSRA = 0; // disable ADV

    return ((long)1024 * 110) / val;
}


int16_t get_vcc_ext()
{
    // get_vcc_ext uses the internal reference 1.1V and measures voltage on a pin
    // Pin has a 1M resistor to battery voltage and 100k resistor to ground
    // Ie pin has 0.090909 times battery voltage, getting compared to 1.1V

    sbi(ADCSRA, ADEN); // switch Analog to Digitalconverter ON
    ADMUX = _BV(MUX1) | _BV(MUX0) | _BV(REFS1); // ADC3 on PB3, which is pin 3; against 1.1V internal reference
    // delay(2);
    ADCSRA |= _BV(ADSC); // Convert
    while (bit_is_set(ADCSRA, ADSC))
        ;
    uint8_t low = ADCL;
    unsigned int val = (ADCH << 8) | low;
    //discard previous result
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
