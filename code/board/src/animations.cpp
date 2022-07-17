#include <Arduino.h>
#include "animations.h"
#include "globals.h"
#include "sleep.h"
#include "vcc.h"
#include "history.h"

extern AnimState as;

const uint8_t daySegs[4] = {
    SG_TR | SG_MD | SG_BL | SG_BR | SG_BM,
    SG_TP | SG_TL | SG_TR | SG_MD | SG_BL | SG_BR,
    SG_TL | SG_TR | SG_MD | SG_BR | SG_BM,
    0};

bool animDay(bool fast)
{
  // At the start, show "dAY"
  // After 1 sec, show # days
  uint8_t action = 0;
  if (as.counter == 0)
    action = 1;
  else if (as.counter == (fast ? 1 * 32 : 1 * 2))
    action = 2;
  if (action != 0)
  {
    ht1621.clearBuffer();
    if (action == 1)
      painter.setAllSegs(daySegs);
    else
    {
      uint16_t days = History::getDayCount();
      uint8_t d3 = days % 10;
      days /= 10;
      uint8_t d2 = days % 10;
      days /= 10;
      uint8_t d1 = days % 10;
      uint8_t d0 = days / 10;
      if (d0)
        painter.setDigit(0, d0);
      if (d0 || d1)
        painter.setDigit(1, d1);
      if (d0 || d1 || d2)
        painter.setDigit(2, d2);
      painter.setDigit(3, d3);
    }
    ht1621.wrBuffer();
  }

  // Do this for about 8 seconds
  ++as.counter;
  return as.counter >= (fast ? 4 * 32 : 4 * 2);
}

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

  uint16_t totalMinutes = (uint16_t)(getCalibratedMsec() / 60000);
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

bool animPastTime(bool fast)
{
  bool draw = as.counter % (fast ? 32 : 2) == 0;
  ++as.counter;

  // This takes exactly 6 seconds
  if (as.counter >= (fast ? 6 * 32 : 6 * 2))
    return true;

  if (draw)
  {
    // 1 second: "d-1", "d-2" or "d-3"
    // 2 seconds: HH:MM
    ht1621.clearBuffer();
    uint8_t secs = as.counter / (fast ? 32 : 2);
    uint8_t daysAgo = 1 + secs / 2;
    if (secs % 2 == 0)
    {
      painter.setSegs(0, SG_TR | SG_MD | SG_BL | SG_BR | SG_BM);
      painter.setSegs(1, SG_MD);
      painter.setDigit(2, daysAgo);
    }
    else
    {
      uint16_t totalMinutes = History::getActiveMinutes(daysAgo);
      uint16_t hours = totalMinutes / 60;
      uint16_t mins = totalMinutes % 60;
      drawTime(hours, mins, true);
    }
    ht1621.wrBuffer();
  }
  return false;
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
    ht1621.clearBuffer();
    painter.setAllSegs(smileyBaseSegs);
    ht1621.wrBuffer();
  }
  ++as.counter;

  // Do this for about 8 seconds
  return as.counter >= (fast ? 8 * 32 : 8 * 2);
}
