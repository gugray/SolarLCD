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

void tempDuty(bool midV)
{
  if (ls.tempMeasureCycle == 0)
  {
    startTempConversion();
    ++ls.tempMeasureCycle;
  }
  else if (ls.tempMeasureCycle < 100 && ls.tempMeasureCycle > (midV ? 4 : 64))
  {
    readTemp();
    ls.tempMeasureCycle += 100;
  }
  else if (ls.tempMeasureCycle > (midV ? MIDV_TEMP_MEASURE_N_HALFSEC : HIGHV_TEMP_MEASURE_N_32MSEC) + 100)
    ls.tempMeasureCycle = 0;
  else
    ++ls.tempMeasureCycle;
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

  // Convert temperature periodically; read result 1 second later
  tempDuty(true);

  bool changeAnim = false;
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

  tempDuty(false);
  
  bool changeAnim = false;
  if (ls.animIx == 0)
    changeAnim = animVoltage(true);
  else if (ls.animIx == 1)
    changeAnim = animTemp(true);
  else if (ls.animIx == 2)
    changeAnim = animTime(true);
  else if (ls.animIx == 3)
    changeAnim = animSmiley(true);
  else if (ls.animIx == 4)
    changeAnim = animEqualizer();
  else if (ls.animIx == 5)
    changeAnim = animLifeGame();
  else if (ls.animIx == 6)
    changeAnim = animEuclideanO();
  else if (ls.animIx == 7)
    changeAnim = animEuclideanU();
  else
    changeAnim = true;

  if (changeAnim)
  {
    as.clear();
    // Voltage -> Temp -> Time -> [Random long]
    if (ls.animIx == 2)
      ls.animIx = 3 + random(5);
    else if (ls.animIx < 2)
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
