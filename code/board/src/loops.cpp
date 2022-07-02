#include <Arduino.h>
#include "loops.h"
#include "animations.h"
#include "globals.h"
#include "sleep.h"
#include "vcc.h"
#include "magic.h"

#define VCC_MEASURE_N_HALFSEC   1200  // Every 10 minutes
#define VCC_MEASURE_N_32MSEC    15    // Every 480 msec ~ half sec

LoopFun currentLoopFun;
LoopState ls;

void measureVcc()
{
    ls.lastVcc = vcc;
    vcc = measure_vcc_ext();
    // vcc = 420;
    ls.vccMeasureCycle = 0;
}

void setLoopFun(LoopFun fun)
{
  currentLoopFun = fun;
  ls.vccMeasureCycle = 1;
  as.clear();
}

void startupLoop()
{
  measureVcc();
  ls.lastVcc = vcc;
  msec = 0;

  if (vcc < MID_VCC_THRESHOLD)
  {
    setLoopFun(lowVoltageLoop);
    // Large value (7 weeks, but whatever)
    // This way transition to mid-voltage will reset counter to zero
    msec = (uint32_t)1000 * 60 * 60 * 168;
  }
  else {
    randomSeed(vcc);
    if (vcc < HIGH_VCC_THRESHOLD)
      setLoopFun(midVoltageLoop);
    else
      setLoopFun(highVoltageLoop);
    }
}

void lowVoltageLoop()
{
  if (ls.vccMeasureCycle >= VCC_MEASURE_N_HALFSEC)
    measureVcc();

  // If voltage is now above threshold: change mode
  if (vcc > MID_VCC_THRESHOLD + VCC_HALF_HYSTERESIS)
  {
    randomSeed(msec);
    setLoopFun(midVoltageLoop);
    // If msec counter is low-ish (<8 hours), we assume this is a fluctuation within the same day
    // If the counter is larger (and it starts super large after wake-up), then we reset it here
    // This also resets counter if voltage dropped to low, but never low enough to reset
    if (msec > (uint32_t)1000 * 60 * 8)
      msec = 0;
    return;
  }

  // Screen off!
  ht1621.setEnabled(false);

  // Blink, unless voltage is growing
  if (vcc <= ls.lastVcc)
  {
    digitalWrite(PIN_LED, HIGH);
    delay(8); // Minimum sleep is 16msec. Running MCU is less costly than flash 2x as long
    msec += 8;
    digitalWrite(PIN_LED, LOW);
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
  if (ls.animIx == 0)
    changeAnim = animVoltage(false);
  else if (ls.animIx == 1)
    changeAnim = animTime(false);
  else if (ls.animIx == 2)
    changeAnim = animSmiley();
  if (changeAnim)
  {
    as.clear();
    ++ls.animIx;
    if (ls.animIx > 2)
      ls.animIx = 0;
  }

  sleep(5); // 500 msec
}

void highVoltageLoop()
{
  if (ls.vccMeasureCycle >= VCC_MEASURE_N_32MSEC)
    measureVcc();

  // If voltage is now below threshold: change mode
  if (vcc < HIGH_VCC_THRESHOLD - VCC_HALF_HYSTERESIS)
  {
    setLoopFun(midVoltageLoop);
    return;
  }

  bool changeAnim = false;
  if (ls.animIx == 0)
    changeAnim = animVoltage(true);
  else if (ls.animIx == 1)
    changeAnim = animTime(true);
  else if (ls.animIx == 2)
    changeAnim = animSmiley();
  else if (ls.animIx == 3)
    changeAnim = animEqualizer();
  else if (ls.animIx == 4)
    changeAnim = animLifeGame();
  else if (ls.animIx == 5)
    changeAnim = animEuclideanO();
  else if (ls.animIx == 6)
    changeAnim = animEuclideanU();

  if (changeAnim)
  {
    as.clear();
    // Voltage -> Time -> [Random long]
    if (ls.animIx == 1)
      ls.animIx = 2 + random(5);
    else if (ls.animIx < 1)
      ls.animIx += 1;
    else
      ls.animIx = 0;
  }

  // Sleep 32 msec, increase counters
  sleep(1);
  ++ls.vccMeasureCycle;
}

// Clock prescaler
// CLKPR = (1 << CLKPCE);
// CLKPR = 0x00;
