#ifndef MODE_LOOPS_H
#define MODE_LOOPS_H

typedef void (*LoopFun)();
extern LoopFun currentLoopFun;

struct LoopState
{
  // Last measured VCC, so we know if level is sinking or growing
  int16_t lastVcc = 0;

  // Number of loops since last VCC measurement
  // Cycle is 8 sec / 500 msec / 32 msec
  uint16_t vccMeasureCycle = 0;

  // Number of cycles since last temperature measurement was started
  // Cycle is 8 sec / 500 msec / 32 msec
  // 0: Start measurement now!
  // 100 or above: has been read; subtract 100 and add 1 second
  uint16_t tempMeasureCycle = 0;

  // Index of animation being played
  uint8_t animIx = 0;
};

extern LoopState ls;

void startupLoop();
void lowVoltageLoop();
void midVoltageLoop();
void highVoltageLoop();

#endif
