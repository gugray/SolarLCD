#include <Arduino.h>
#include "loops.h"
#include "animations.h"
#include "globals.h"
#include "sleep.h"
#include "vcc.h"
#include "temp.h"
#include "magic.h"

#define LOWV_VCC_MEASURE_N_HALFSEC 1200 // Every 10 minutes
#define HIGHV_VCC_MEASURE_N_32MSEC 15   // Every 480 msec ~ half sec

#define MIDV_TEMP_MEASURE_N_HALFSEC 240 // Mid-voltage mode: every 2 minutes
#define HIGHV_TEMP_MEASURE_N_32MSEC 938 // High-voltage mode: every 30 seconds

LoopFun currentLoopFun;
LoopState ls;

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
  ls.tempMeasureCycle = 0;
  as.clear();
  ls.animIx = 0;
}

void tempDuty(bool midVoltage, bool measureIfOldEnough)
{
  // Read out temperature 2 seconds after we started measurement
  if (ls.tempMeasureCycle < 100 && ls.tempMeasureCycle > (midVoltage ? 4 : 64))
  {
    readTemp();
    ls.tempMeasureCycle += 100;
  }
  else if (measureIfOldEnough && ls.tempMeasureCycle > (midVoltage ? MIDV_TEMP_MEASURE_N_HALFSEC : HIGHV_TEMP_MEASURE_N_32MSEC) + 100)
  {
    ls.tempMeasureCycle = 0;
    startTempConversion();
  }
  else
  {
    ++ls.tempMeasureCycle;
  }
}

void startupLoop()
{
  // measureTemp(); // DBG
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
    pinMode(PIN_LED, OUTPUT);
    digitalWrite(PIN_LED, LOW);
    delay(8); // Minimum sleep is 16msec. Running MCU is less costly than flash 2x as long
    msec += 8;
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
  bool canMeasureTemp = false;
  if (ls.animIx == 0)
    changeAnim = animVoltage(false);
  else if (ls.animIx == 1)
    changeAnim = animTemp(false);
  else if (ls.animIx == 2)
    changeAnim = animTime(false);
  else if (ls.animIx == 3)
    changeAnim = animSmiley(false);
  else
    changeAnim = true;
  if (changeAnim)
  {
    as.clear();
    ++ls.animIx;
    if (ls.animIx > 3)
    {
      ls.animIx = 0;
      // We start temp converstion when showing voltage
      // So we'll have a fresh value to show in next phase
      canMeasureTemp = true;
    }
  }

  // Convert temperature periodically; read result a bit later
  tempDuty(true, canMeasureTemp);

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
  bool canMeasureTemp = false;
  if (ls.animIx == 0)
    changeAnim = animVoltage(true);
  else if (ls.animIx == 1)
    changeAnim = animTemp(true);
  else if (ls.animIx == 2)
    changeAnim = animTime(true);
  else if (ls.animIx == 3)
    changeAnim = animSmiley(true);
  else if (ls.animIx == 4 || ls.animIx == 5)
    changeAnim = animEqualizer();
  else if (ls.animIx == 6 || ls.animIx == 7)
    changeAnim = animEuclidean(true);
  else if (ls.animIx == 8 || ls.animIx == 9)
    changeAnim = animEuclidean(false);
  else if (ls.animIx == 10)
    changeAnim = animLifeGame();
  else
    changeAnim = true;

  if (changeAnim)
  {
    as.clear();
    // Voltage -> Temp -> Time -> [Random long]
    if (ls.animIx == 2)
      ls.animIx = random(3, 11);
    else if (ls.animIx < 2)
      ls.animIx += 1;
    else
    {
      ls.animIx = 0;
      canMeasureTemp = true;
    }
  }

  tempDuty(false, canMeasureTemp);

  // Sleep 32 msec, increase counters
  sleep(1);
  ++ls.vccMeasureCycle;
}
