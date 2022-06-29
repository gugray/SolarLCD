#include <Arduino.h>
#include "animations.h"
#include "globals.h"
#include "sleep.h"
#include "vcc.h"

AnimState as;

AnimState::AnimState()
{
  clear();
}

void AnimState::clear()
{
  memset(this, 0, sizeof(AnimState));
}

bool AnimState::getFlag(uint8_t ix)
{
  uint16_t val = flags & (1 << ix);
  return val != 0;
}

void AnimState::setFlag(uint8_t ix, bool val)
{
  if (val)
    flags |= (1 << ix);
  else
  {
    uint16_t mask = 0xffff - (1 << ix);
    flags = flags & mask;
  }
}

void drawVoltage(int16_t val)
{
  painter.setDigit(3, val % 10);
  val /= 10;
  painter.setDigit(2, val % 10);
  val /= 10;
  painter.setDigit(0, val);
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
    painter.setSegs(1, SG_BL | SG_BR | SG_BM);
  
  ht1621.wrBuffer();

  // Do this for about 8 seconds
  return as.counter >= (fast ? 8 * 32 : 8 * 2);
}


void drawTime(int16_t hours, int16_t mins, bool showColon)
{
  painter.setDigit(1, hours % 10);
  hours /= 10;
  if (hours != 0) painter.setDigit(0, hours);

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

  uint16_t totalMinutes = (uint16_t)(msec / 60000);
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

