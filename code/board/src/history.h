#ifndef HISTORY_H
#define HISTORY_H

struct History
{
  static void newDay();
  static void setActiveMinutes(uint16_t minutesAwake);
  static uint16_t getActiveMinutes(uint8_t daysAgo);
  static uint16_t getDayCount();
  static void reset(bool sampleData);
};

#endif
