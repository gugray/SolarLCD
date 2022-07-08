#include <Arduino.h>
#include <EEPROM.h>
#include "history.h"

/* EEPROM layout

[0:1]   day counter
[2:3]   total minutes awake today (update when entering low voltage from above)
[4:5]   total minutes awake D-1
[6:7]   total minutes awake D-2
[8:9]   total minutes awake D-3

*/

uint16_t dayCount = 0xffff;

void History::reset(bool sampleData)
{
  if (sampleData)
  {
    dayCount = 953;
    EEPROM.put(0, (uint16_t)dayCount);
    EEPROM.put(2, (uint16_t)72);
    EEPROM.put(4, (uint16_t)623);
    EEPROM.put(6, (uint16_t)514);
    EEPROM.put(8, (uint16_t)590);
  }
  else
  {
    dayCount = 0;
    EEPROM.put(0, (uint16_t)dayCount);
    EEPROM.put(2, (uint16_t)0);
    EEPROM.put(4, (uint16_t)0);
    EEPROM.put(6, (uint16_t)0);
    EEPROM.put(8, (uint16_t)0);
  }
}

void History::newDay()
{
  // Increase day count
  // Shift previous days to the right
  // Write 0 minutes for today
  EEPROM.get(0, dayCount);
  ++dayCount;
  EEPROM.put(0, dayCount);
  uint16_t val;
  for (uint8_t i = 2; i < 8; i += 2)
  {
    EEPROM.get(i, val);
    EEPROM.put(i + 2, val);
  }
  EEPROM.put(2, (uint16_t)0);
}

uint16_t History::getDayCount()
{
  if (dayCount == 0xffff)
    EEPROM.get(0, dayCount);
  return dayCount;
}

void History::setActiveMinutes(uint16_t minutesAwake)
{
  EEPROM.put(2, (uint16_t)minutesAwake);
}

uint16_t History::getActiveMinutes(uint8_t daysAgo)
{
  uint16_t val;
  EEPROM.get(2 + daysAgo * 2, val);
  return val;
}