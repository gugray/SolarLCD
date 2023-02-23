#ifndef MAGIC_H
#define MAGIC_H

#define VCC_HALF_HYSTERESIS   3
#define MID_VCC_THRESHOLD     328
#define HIGH_VCC_THRESHOLD    410

// It's a new day if creature rises into mid-voltage after at least
// this number of minutes in low.
#define NEWDAY_MIN_MINUTES_IN_LOW  240

// To be empirically determined for every unit.
// First large unit: 23/20
// First small unit: 1/1
#define TIME_CORR_NUMERATOR   1
#define TIME_CORR_DENOMINATOR 1

#ifdef ENV_ATTINY
#define PIN_CS    4
#define PIN_WR    2
#define PIN_DATA  1
#define PIN_LED   0
#endif

#ifdef ENV_NANO
#define PIN_CS    11
#define PIN_WR    12
#define PIN_DATA  7
#define PIN_LED   13
#endif

#endif
