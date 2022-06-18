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
void smileyLoop();
void blinkeyLoop();
void equalizerLoop();
void vccLoop();


void startupLoop()
{
  measureVcc();
  if (vcc < MID_VCC_THRESHOLD)
    currentLoopFun = lowVoltageLoop;
  else if (vcc < HIGH_VCC_THRESHOLD)
    currentLoopFun = midVoltageLoop;
  else
    currentLoopFun = highVoltageLoop;

  // DBG
  currentLoopFun = equalizerLoop;

  // currentLoopFun = blinkeyLoop;
  // ht1621.setEnabled(false);

  // currentLoopFun = vccLoop;
}

void drawVoltage(int num, bool showDecimalPoint)
{
  int val = num;
  int n = val % 10;
  painter.setDigit(3, n);
  val /= 10;
  n = val % 10;
  if (val != 0) painter.setDigit(2, n);
  val /= 10;
  n = val % 10;
  if (val != 0) painter.setDigit(1, n);
  val /= 10;
  n = val % 10;
  if (val != 0) painter.setDigit(0, n);

  if (showDecimalPoint) painter.setDots(DT_MDEC);
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
  if (dotShown) painter.setDots(DT_MDOT);
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
    if (dotShown) painter.setDots(DT_MDOT);
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
  if (dotShown) painter.setDots(DT_MDOT);
  ht1621.wrBuffer();
  sleep(5); // 500 msec
}


void smileyLoop()
{
  ht1621.clearBuffer();
  painter.setImage(Painter::frog);
  ht1621.wrBuffer();
  sleep(5); // 500 msec

  ht1621.clearBuffer();
  painter.setImage(Painter::helo);
  ht1621.wrBuffer();
  sleep(5); // 500 msec
}

#define NOISE_LEN 8 
uint8_t noise1[NOISE_LEN] = { 7, 69, 98, 87, 37, 88, 93, 110 }; 
uint8_t noise2[NOISE_LEN] = { 7, 94, 123, 87, 37, 63, 119, 110 }; 
uint8_t noise3[NOISE_LEN] = { 2, 69, 123, 56, 36, 37, 123, 106 }; 

void drawEqualizerBar(uint8_t *buf, uint8_t segMask)
{
  uint8_t nVals = NOISE_LEN * 3;
  uint8_t ix = counter % nVals;
  uint8_t bufPos = ix / 3;
  uint8_t ofs = ix % 3;
  uint8_t cval = buf[bufPos];
  uint8_t val;
  uint8_t val0 = cval / 25;
  if (ofs == 0) val = val0;
  else
  {
    uint8_t val1 = (cval - val0 * 25) / 5;
    if (ofs == 1) val = val1;
    else
    {
      uint8_t val2 = cval - val0 * 25 - val1 * 5;
      val = val2;
    }
  }

  // if (segMask == SG_TP) painter.setDigit(0, val);
  // else if (segMask == SG_MD) painter.setDigit(1, val);
  // else painter.setDigit(2, val);

  if (val > 0) painter.setSegs(0, segMask);
  if (val > 1) painter.setSegs(1, segMask);
  if (val > 2) painter.setSegs(2, segMask);
  if (val > 3) painter.setSegs(3, segMask);
}

void equalizerLoop() {

  ht1621.clearBuffer();
  drawEqualizerBar(noise1, SG_TP);
  drawEqualizerBar(noise2, SG_MD);
  drawEqualizerBar(noise3, SG_BM);
  ht1621.wrBuffer();

  ++counter;
  sleep(3);
}
