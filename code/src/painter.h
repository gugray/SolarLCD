#ifndef PAINTER_H
#define PAINTER_H

class Painter
{
private:
  uint8_t *buf;

public:
  Painter(uint8_t *buf);
  void setDigit(int num, int rofs);

  // Turns on decimal digit or dot.
  // 0: degree sign; 1: mid dot; 2: mid decimal; 3: left decimal
  void setDot(uint8_t dot);
};

#endif
