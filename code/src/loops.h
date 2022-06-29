#ifndef MODE_LOOPS_H
#define MODE_LOOPS_H

typedef void (*LoopFun)();
extern LoopFun currentLoopFun;

struct LoopState
{
  // Last measured VCC, so we know if level is sinking or growing
  int16_t lastVcc = 0;

  // Number of loops since last VCC measurement
  // In low-voltage loop, cycles are 8 sec, and measurements very rare
  // In mid-voltage loop, cycle is 500 msec and we measure every cycle, so no counting
  // In high-voltage loop, cycles are 32 msec
  uint16_t vccMeasureCycle = 0;

  // Index of animation being played
  uint8_t animIx = 0;
};

extern LoopState ls;

void startupLoop();
void lowVoltageLoop();
void midVoltageLoop();
void highVoltageLoop();

#endif
