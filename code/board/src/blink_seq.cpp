#include <Arduino.h>
#include "blink_seq.h"
#include "euclidean.h"
#include "sleep.h"
#include "magic.h"

uint16_t rhythm = 0;
uint8_t sleepHalfSec = 8;

void addRhythm(uint8_t nBeats, uint8_t ofs)
{
  for (uint8_t i = 0; i < 16; ++i)
  {
    if (isOnBeat(nBeats, ofs, i))
    {
      uint16_t mask = 0x8000 >> i;
      rhythm |= mask;
    }
  }
}

void initBlinkSeq()
{
  rhythm = 0;
  addRhythm(random(1, 4), random(0, 4));
  addRhythm(random(1, 4), random(0, 4));
  uint8_t nBlinks = 0;
  for (uint16_t mask = 0x8000; mask != 0; mask >>= 1)
  {
    if ((rhythm & mask) != 0)
      ++nBlinks;
  }
  sleepHalfSec = 4 + nBlinks * 3 / 2;
}

uint16_t blinkOneSeq()
{
  // Play rhythm: 16 beats, 4 beats per second -> 4 seconds
  for (uint8_t i = 0; i < 16; ++i)
  {
    // Blink here?
    uint16_t mask = 0x8000 >> i;
    if ((rhythm & mask) != 0)
    {
      pinMode(PIN_LED, OUTPUT);
      digitalWrite(PIN_LED, LOW);
      delay(8); // Minimum sleep is 16msec. Running MCU is less costly than flash 2x as long
      addRawMsec(8);
      digitalWrite(PIN_LED, HIGH);
      pinMode(PIN_LED, INPUT);
    }
    // Sleep 1/4 seconds
    sleep(4);
  }

  // Sleep in increments
  uint8_t sleepyTime = sleepHalfSec;
  // How many two-second sleeps?
  uint8_t twoSecs = sleepyTime / 4;
  for (uint8_t i = 0; i < twoSecs; ++i)
    sleep(7);
  // How many one-second sleeps?
  sleepyTime = sleepyTime % 4;
  uint8_t secs = sleepyTime / 2;
  for (uint8_t i = 0; i < secs; ++i)
    sleep(6);
  // The rest in half seconds
  sleepyTime = sleepyTime % 2;
  if (sleepyTime != 0)
    sleep(7);
  
  // Time this whole thing took, the 16-note cycle plus the sleep after
  return 8 + sleepHalfSec;
}