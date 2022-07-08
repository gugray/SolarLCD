#include <Arduino.h>
#include "animations.h"
#include "globals.h"
#include "sleep.h"
#include "vcc.h"
#include "temp.h"

extern AnimState as;

void drawVoltage(int16_t val)
{
  painter.setDigit(2, val % 10);
  val /= 10;
  painter.setDigit(1, val % 10);
  val /= 10;
  painter.setDigit(0, val);
  painter.setSegs(3, SG_BL | SG_BR | SG_BM);
}

bool animVoltage(bool fast)
{
  if (fast)
  {
    // We get called at 32 msec intervals, which is about 32 times per second
    // We do something every 8th time, i.e. about 4 times per second
    if ((as.counter++ % 8) != 0)
      return false;
  }
  else
    as.counter++;

  ht1621.clearBuffer();
  ht1621.setEnabled(true);
  drawVoltage(vcc);
  bool dotShown = !as.getFlag(1);
  as.setFlag(1, dotShown);
  if (dotShown)
    painter.setDots(DT_LDEC);

  ht1621.wrBuffer();

  // Do this for about 8 seconds
  return as.counter >= (fast ? 8 * 32 : 8 * 2);
}

void drawTime(int16_t hours, int16_t mins, bool showColon)
{
  painter.setDigit(1, hours % 10);
  hours /= 10;
  if (hours != 0)
    painter.setDigit(0, hours);

  painter.setDigit(3, mins % 10);
  painter.setDigit(2, mins / 10);

  if (showColon)
    painter.setDots(DT_MDEC | DT_MDOT);
}

bool animTime(bool fast)
{
  if (fast)
  {
    // We get called at 32 msec intervals, which is about 32 times per second
    // We do something every 8th time, i.e. about 4 times per second
    if ((as.counter++ % 8) != 0)
      return false;
  }
  else
    as.counter++;

  // 60000, but correcting with 1.15 (15%) more real time than measured
  // => 52174
  uint16_t totalMinutes = (uint16_t)(msec / 52174);
  uint16_t hours = totalMinutes / 60;
  uint16_t mins = totalMinutes % 60;
  bool colonShown = !as.getFlag(1);
  as.setFlag(1, colonShown);

  ht1621.clearBuffer();
  ht1621.setEnabled(true);

  drawTime(hours, mins, colonShown);

  ht1621.wrBuffer();

  // Do this for about 8 seconds
  return as.counter >= (fast ? 8 * 32 : 8 * 2);
}

void drawTemp(uint16_t val, bool degreeShown, bool fahrenheit)
{
  uint8_t d2 = val % 10;
  val /= 10;
  uint8_t d1 = val % 10;
  val /= 10;
  uint8_t d0 = val;
  if (d0 != 0)
    painter.setDigit(0, d0);
  if (d0 != 0 || d1 != 0)
    painter.setDigit(1, d1);
  painter.setDigit(2, d2);

  if (degreeShown)
    painter.setDots(DT_DEGR);
  if (fahrenheit)
    painter.setSegs(3, SG_TP | SG_TL | SG_MD | SG_BL);
  else
  {
    painter.setSegs(3, SG_TP | SG_TL | SG_BL | SG_BM);
    painter.setDots(DT_MDEC);
  }
}

bool animTemp(bool fast)
{
  if (fast)
  {
    // We get called at 32 msec intervals, which is about 32 times per second
    // We do something every 8th time, i.e. about 4 times per second
    if ((as.counter++ % 8) != 0)
      return false;
  }
  else
    as.counter++;

  bool degreeShown = !as.getFlag(1);
  as.setFlag(1, degreeShown);

  bool fahrenheit = false;
  if (fast && as.counter >= 4 * 32)
    fahrenheit = true;
  else if (!fast && as.counter >= 4 * 2)
    fahrenheit = true;

  ht1621.clearBuffer();
  ht1621.setEnabled(true);
  if (fahrenheit)
    drawTemp((uint16_t)tempF, degreeShown, true);
  else
    drawTemp((uint16_t)tempC, degreeShown, false);
  ht1621.wrBuffer();

  // Do this for about 8 seconds
  return as.counter >= (fast ? 8 * 32 : 8 * 2);
}

const uint8_t smileyBaseSegs[4] = {
    SG_BL | SG_BM,
    SG_TP | SG_TL | SG_TR | SG_MD | SG_BM,
    SG_TP | SG_TL | SG_TR | SG_MD | SG_BM,
    SG_BR | SG_BM};

bool animSmiley(bool fast)
{
  if (as.counter == 0)
  {
    painter.setAllSegs(smileyBaseSegs);
    ht1621.wrBuffer();
  }
  ++as.counter;

  // Do this for about 8 seconds
  return as.counter >= (fast ? 8 * 32 : 8 * 2);
}
