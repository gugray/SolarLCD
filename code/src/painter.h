#ifndef PAINTER_H
#define PAINTER_H

class Painter
{
private:
  uint8_t *buf;

public:
  Painter(uint8_t *buf);
  void drawDigit(int num, int rofs);
};

#endif
