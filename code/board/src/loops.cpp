#include <Arduino.h>
#include "loops.h"
#include "animations.h"
#include "globals.h"
#include "sleep.h"
#include "vcc.h"
#include "history.h"
#include "magic.h"

#define LOWV_VCC_MEASURE_N_HALFSEC 1200 // Every 10 minutes
#define HIGHV_VCC_MEASURE_N_32MSEC 15   // Every 480 msec ~ half sec

LoopFun currentLoopFun;
LoopState ls;

typedef bool (*AnimFun)(bool);

#define N_BASIC_FAST_ANIMS 4
#define N_FAST_ANIMS 9
const AnimFun fastAnimFuns[N_FAST_ANIMS] = { animDay, animVoltage, animTime, animPastTime,
  animEuclideanO, animEuclideanO,
  animEuclideanU, animEuclideanU,
  animLifeGame
};

#define N_SLOW_ANIMS 5
const AnimFun slowAnimFuns[] = {animDay, animVoltage, animTime, animPastTime, animSmiley};


void measureVcc()
{
  ls.lastVcc = vcc;
  measureVCC();
  // vcc = 250;
  ls.vccMeasureCycle = 0;
}

void setLoopFun(LoopFun fun)
{
  currentLoopFun = fun;
  ls.vccMeasureCycle = 1;
  as.clear();
  ls.animIx = 0;
}

void startupLoop()
{
  // History::reset(false); // DBG
  
  measureVcc();
  ls.lastVcc = vcc;
  setRawMsec(0);

  if (vcc < MID_VCC_THRESHOLD)
  {
    setLoopFun(lowVoltageLoop);
    // Large value (7 weeks, but whatever)
    // This way transition to mid-voltage will reset counter to zero
    setRawMsec((uint32_t)1000 * 60 * 60 * 168);
  }
  else
  {
    randomSeed(vcc);
    if (vcc < HIGH_VCC_THRESHOLD)
      setLoopFun(midVoltageLoop);
    else
      setLoopFun(highVoltageLoop);
  }
}

void lowVoltageLoop()
{
  if (ls.vccMeasureCycle >= LOWV_VCC_MEASURE_N_HALFSEC)
    measureVcc();

  // If voltage is now above threshold: change mode
  if (vcc > MID_VCC_THRESHOLD + VCC_HALF_HYSTERESIS)
  {
    uint32_t msec = getCalibratedMsec();
    randomSeed(msec);
    setLoopFun(midVoltageLoop);
    // If msec counter is low-ish (<8 hours), we assume this is a fluctuation within the same day
    // If the counter is larger (and it starts super large after wake-up), then we reset it here
    // This also resets counter if voltage dropped to low, but never low enough to reset
    if (msec > (uint32_t)1000 * 60 * 8)
    {
      msec = 0;
      History::newDay();
    }
    return;
  }

  // Screen off!
  ht1621.setEnabled(false);

  // Blink, unless voltage is growing
  if (vcc <= ls.lastVcc)
  {
    pinMode(PIN_LED, OUTPUT);
    digitalWrite(PIN_LED, LOW);
    delay(8); // Minimum sleep is 16msec. Running MCU is less costly than flash 2x as long
    addRawMsec(8);
    digitalWrite(PIN_LED, HIGH);
    pinMode(PIN_LED, INPUT);
  }

  // Sleep
  ls.vccMeasureCycle += 8;
  sleep(8); // 4 sec
}

void midVoltageLoop()
{
  measureVcc();

  // If voltage is now below threshold: change mode
  if (vcc < MID_VCC_THRESHOLD - VCC_HALF_HYSTERESIS)
  {
    History::setActiveMinutes(getCalibratedMsec() / 60000);
    setLoopFun(lowVoltageLoop);
    return;
  }
  // If voltage is now above high threshold: change mode
  if (vcc > HIGH_VCC_THRESHOLD + VCC_HALF_HYSTERESIS)
  {
    setLoopFun(highVoltageLoop);
    return;
  }

  bool changeAnim = false;
  if (ls.animIx < N_SLOW_ANIMS)
    changeAnim = slowAnimFuns[ls.animIx](false);
  else
    changeAnim = true;
  if (changeAnim)
  {
    as.clear();
    ++ls.animIx;
    if (ls.animIx >= N_SLOW_ANIMS)
      ls.animIx = 0;
  }

  sleep(5); // 500 msec
}

void highVoltageLoop()
{
  if (ls.vccMeasureCycle >= HIGHV_VCC_MEASURE_N_32MSEC)
    measureVcc();

  // If voltage is now below threshold: change mode
  if (vcc < HIGH_VCC_THRESHOLD - VCC_HALF_HYSTERESIS)
  {
    setLoopFun(midVoltageLoop);
    return;
  }

  bool changeAnim = false;
  if (ls.animIx < N_FAST_ANIMS)
    changeAnim = fastAnimFuns[ls.animIx](true);
  else
    changeAnim = true;
  if (changeAnim)
  {
    as.clear();
    // [Basic sequence] -> [1 random long]
    if (ls.animIx == N_BASIC_FAST_ANIMS - 1)
      ls.animIx = random(N_BASIC_FAST_ANIMS, N_FAST_ANIMS + 1);
    else if (ls.animIx < N_BASIC_FAST_ANIMS - 1)
      ls.animIx += 1;
    else
      ls.animIx = 0;
  }

  // Sleep 32 msec, increase counters
  sleep(1);
  ++ls.vccMeasureCycle;
}
