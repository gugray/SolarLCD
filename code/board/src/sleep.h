#ifndef SLEEP_H
#define SLEEP_H

// Gets the number of msecs elapsed, adjusted by the empirical correction factor
// to roughly compensate for this unit's inaccuracy.
uint32_t getCalibratedMsec();

// Directly sets raw msec counter.
void setRawMsec(uint32_t val);

// Adds a value to the raw (uncalibrated) msec counter.
void addRawMsec(uint32_t val);

// Sleeps for desired length of time, then returns
// 0=16ms, 1=32ms, 2=64ms, 3=128ms, 4=250ms, 5=500ms, 6=1 sec, 7=2 sec, 8=4 sec, 9=8sec
void sleep(uint8_t sleepLength);

#endif
