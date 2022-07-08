#include <Arduino.h>
#include "animations.h"

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
