#ifndef SHOWS_H
#define SHOWS_H

#define AS_MEM_SZ 32

struct AnimState
{
  uint16_t flags;
  uint16_t counter;
  uint16_t a;
  uint16_t b;
  uint16_t c;
  uint16_t d;
  uint8_t mem[AS_MEM_SZ];

  AnimState();
  void clear();
  bool getFlag(uint8_t ix);
  void setFlag(uint8_t ix, bool val);
};

extern AnimState as;

bool animVoltage(bool fast);
bool animTime(bool fast);
bool animEqualizer();
bool animLifeGame();
bool animEuclideanO();
bool animEuclideanU();
bool animSmiley();

#endif
