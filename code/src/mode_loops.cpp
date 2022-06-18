#include <Arduino.h>
#include "mode_loops.h"
#include "globals.h"
#include "sleep.h"
#include "vcc.h"
#include "magic.h"

#define VCC_MEASURE_N_HALFSEC   1200  // Every 10 minutes
#define VCC_MEASURE_N_32MSEC    15    // Every 480 msec ~ half sec

LoopFun currentLoopFun;
bool lcdOn = true;
bool dotShown = false;
int16_t lastVcc = 0, vcc = 0;
uint16_t counter = 0;

// Number of loops since last measurement
// In low-voltage loop, cycles are 8 sec, and measurements very rare
// In high-voltage loop, cycles are 32 msec
uint16_t vccMeasureCycle = 0;

void measureVcc();

void startupLoop()
{
  measureVcc();
  if (vcc < MID_VCC_THRESHOLD)
    currentLoopFun = lowVoltageLoop;
  else if (vcc < HIGH_VCC_THRESHOLD)
    currentLoopFun = midVoltageLoop;
  else
    currentLoopFun = highVoltageLoop;

  // // DBG
  // currentLoopFun = blinkeyLoop;
  // ht1621.setEnabled(false);

  // // DBG
  // currentLoopFun = vccLoop;
}

void drawVoltage(int num, bool showDecimalPoint)
{
  int val = num;
  int n = val % 10;
  painter.setDigit(n, 0);
  val /= 10;
  n = val % 10;
  if (val != 0) painter.setDigit(n, 1);
  val /= 10;
  n = val % 10;
  if (val != 0) painter.setDigit(n, 2);
  val /= 10;
  n = val % 10;
  if (val != 0) painter.setDigit(n, 3);

  if (showDecimalPoint) painter.setDot(2);
}

void measureVcc()
{
    lastVcc = vcc;
    vcc = get_vcc_ext();
    //vcc = get_vcc();
    vccMeasureCycle = 0;
}

void lowVoltageLoop()
{
  if (vccMeasureCycle >= VCC_MEASURE_N_HALFSEC)
    measureVcc();

  // If voltage is now above threshold: change mode
  if (vcc > MID_VCC_THRESHOLD + VCC_HALF_HYSTERESIS)
  {
    currentLoopFun = midVoltageLoop;
    return;
  }

  // Blink, unless voltage is growing
  if (vcc <= lastVcc)
  {
    digitalWrite(PIN_LED, HIGH);
    delay(8);
    digitalWrite(PIN_LED, LOW);
  }

  // Sleep
  vccMeasureCycle += 8;
  sleep(8); // 4 sec
}

void midVoltageLoop()
{
  measureVcc();

  // If voltage is now below threshold: change mode
  if (vcc < MID_VCC_THRESHOLD - VCC_HALF_HYSTERESIS)
  {
    currentLoopFun = lowVoltageLoop;
    vccMeasureCycle = 1;
    return;
  }
  // If voltage is now above high threshold: change mode
  if (vcc > HIGH_VCC_THRESHOLD + VCC_HALF_HYSTERESIS)
  {
    currentLoopFun = highVoltageLoop;
    vccMeasureCycle = 1;
    return;
  }

  ht1621.clearBuffer();
  if (!lcdOn) ht1621.setEnabled(true);
  lcdOn = true;
  drawVoltage(vcc, true);
  dotShown = !dotShown;
  if (dotShown) painter.setDot(1);
  ht1621.wrBuffer();
  sleep(5); // 500 msec
}

void highVoltageLoop()
{
  if (vccMeasureCycle >= VCC_MEASURE_N_32MSEC)
    measureVcc();

  // If voltage is now below threshold: change mode
  if (vcc < HIGH_VCC_THRESHOLD - VCC_HALF_HYSTERESIS)
  {
    currentLoopFun = midVoltageLoop;
    vccMeasureCycle = 1;
    return;
  }

  // About four times a second, we update display
  if (counter == 8)
  {
    counter = 0;
    ht1621.clearBuffer();
    if (!lcdOn) ht1621.setEnabled(true);
    lcdOn = true;
    dotShown = !dotShown;
    drawVoltage(vcc, !dotShown);
    if (dotShown) painter.setDot(1);
    ht1621.wrBuffer();
  }

  // Sleep 32 msec, increase counters
  sleep(1);
  ++counter;
  ++vccMeasureCycle;
}

void blinkeyLoop()
{
  digitalWrite(PIN_LED, HIGH);
  delay(8);
  digitalWrite(PIN_LED, LOW);

  sleep(8); // 4 sec
}

// Clock prescaler
// CLKPR = (1 << CLKPCE);
// CLKPR = 0x00;


void vccLoop()
{
  measureVcc();
  ht1621.clearBuffer();
  if (!lcdOn) ht1621.setEnabled(true);
  lcdOn = true;
  drawVoltage(vcc, true);
  dotShown = !dotShown;
  if (dotShown) painter.setDot(1);
  ht1621.wrBuffer();
  sleep(5); // 500 msec
}


void smileyLoop()
{
  ht1621.clearBuffer();
  // Left eye
  painter.setSeg(1, 0);
  painter.setSeg(1, 1);
  painter.setSeg(1, 2);
  painter.setSeg(1, 3);
  // Right eye
  painter.setSeg(2, 0);
  painter.setSeg(2, 1);
  painter.setSeg(2, 2);
  painter.setSeg(2, 3);
  // Mouth
  painter.setSeg(0, 4);
  painter.setSeg(0, 6);
  painter.setSeg(1, 6);
  painter.setSeg(2, 6);
  painter.setSeg(3, 6);
  painter.setSeg(3, 5);

  ht1621.wrBuffer();
  sleep(5); // 500 msec
}
