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

  // Raw msec count when we last sunk into low-voltage mode, or 0 if that hasn't happened yet.
  uint32_t msecSinkIntoLow = 0;

  // Index of animation being played
  uint8_t animIx = 0;
};

extern LoopState ls;

void startupLoop();
void lowVoltageLoop();
void midVoltageLoop();
void highVoltageLoop();

#endif
