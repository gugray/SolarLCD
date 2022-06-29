#ifndef SHOWS_H
#define SHOWS_H

struct AnimState
{
  uint16_t flags;
  uint16_t counter;

  AnimState();
  void clear();
  bool getFlag(uint8_t ix);
  void setFlag(uint8_t ix, bool val);
};

extern AnimState as;

bool animVoltage(bool fast);
bool animTime(bool fast);
bool animEqualizer();

#endif
