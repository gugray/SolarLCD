#ifndef VCC_H
#define VCC_H

// Last measured VCC (volts * 100).
// You're only supposed to read this. measure_vcc_ext() writes it.
extern volatile int16_t vcc;

void measureVCC();

#endif
