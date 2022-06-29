#ifndef SLEEP_H
#define SLEEP_H

// Global msec counter
extern volatile uint32_t msec;

// Sleeps for desired length of time, then returns
// 0=16ms, 1=32ms, 2=64ms, 3=128ms, 4=250ms, 5=500ms, 6=1 sec, 7=2 sec, 8=4 sec, 9=8sec
void sleep(uint8_t sleepLength);

#endif
