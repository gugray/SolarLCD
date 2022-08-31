#include <Arduino.h>
#include "loops.h"
#include "animations.h"
#include "blink_seq.h"
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
  clearMsec();

  if (vcc < MID_VCC_THRESHOLD)
  {
    setLoopFun(lowVoltageLoop);
    ht1621.setEnabled(false);
    initBlinkSeq();
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

    // This is a new day if:
    // -- We haven't sunk into low-voltage mode yet since startup, OR
    // -- The last time we sank into low-voltage mode has been more than N minutes ago (~240 min)
    bool newDay = ls.msecSinkIntoLow == 0;
    if (!newDay)
    {
      uint32_t msecSinceLastSink = msec - ls.msecSinkIntoLow;
      if (msecSinceLastSink > (uint32_t)NEWDAY_MIN_MINUTES_IN_LOW * 1000 * 60)
        newDay = true;
    }
    if (newDay)
    {
      clearMsec();
      History::newDay();
    }
    return;
  }

  // If voltage is growing, just sleep
  if (vcc > ls.lastVcc) {
    ls.vccMeasureCycle += 8;
    sleep(8); // 4 sec
  }
  // Otherwise, blink a sequence
  else
  {
    ls.vccMeasureCycle += blinkOneSeq();
  }
}

void midVoltageLoop()
{
  measureVcc();

  // If voltage is now below threshold: change mode
  if (vcc < MID_VCC_THRESHOLD - VCC_HALF_HYSTERESIS)
  {
    History::setActiveMinutes(getCalibratedMsec() / 60000);
    ls.msecSinkIntoLow = getCalibratedMsec();
    setLoopFun(lowVoltageLoop);
    ht1621.setEnabled(false);
    initBlinkSeq();
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
