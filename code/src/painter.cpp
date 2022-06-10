#include <Arduino.h>
#include "painter.h"

#define D0_B0  0b00000000
#define D0_B1  0b00010001
#define D0_B2  0b00010001
#define D0_B3  0b00010001
#define D1_B0  0b00000000
#define D1_B1  0b00000000
#define D1_B2  0b00010000
#define D1_B3  0b00000001
#define D2_B0  0b00010000
#define D2_B1  0b00010000
#define D2_B2  0b00000001
#define D2_B3  0b00010001
#define D3_B0  0b00010000
#define D3_B1  0b00000000
#define D3_B2  0b00010001
#define D3_B3  0b00010001
#define D4_B0  0b00010000
#define D4_B1  0b00000001
#define D4_B2  0b00010000
#define D4_B3  0b00000001
#define D5_B0  0b00010000
#define D5_B1  0b00000001
#define D5_B2  0b00010001
#define D5_B3  0b00010000
#define D6_B0  0b00010000
#define D6_B1  0b00010001
#define D6_B2  0b00010001
#define D6_B3  0b00010000
#define D7_B0  0b00000000
#define D7_B1  0b00000000
#define D7_B2  0b00010000
#define D7_B3  0b00010001
#define D8_B0  0b00010000
#define D8_B1  0b00010001
#define D8_B2  0b00010001
#define D8_B3  0b00010001
#define D9_B0  0b00010000
#define D9_B1  0b00000001
#define D9_B2  0b00010000
#define D9_B3  0b00010001


Painter::Painter(uint8_t *buf)
: buf(buf)
{
}

void Painter::setDigit(int num, int rofs)
{
  uint8_t b0, b1, b2, b3;
  if (num == 0) { b0 = D0_B0; b1 = D0_B1; b2 = D0_B2; b3 = D0_B3; }
  else if (num == 1) { b0 = D1_B0; b1 = D1_B1; b2 = D1_B2; b3 = D1_B3; }
  else if (num == 2) { b0 = D2_B0; b1 = D2_B1; b2 = D2_B2; b3 = D2_B3; }
  else if (num == 3) { b0 = D3_B0; b1 = D3_B1; b2 = D3_B2; b3 = D3_B3; }
  else if (num == 4) { b0 = D4_B0; b1 = D4_B1; b2 = D4_B2; b3 = D4_B3; }
  else if (num == 5) { b0 = D5_B0; b1 = D5_B1; b2 = D5_B2; b3 = D5_B3; }
  else if (num == 6) { b0 = D6_B0; b1 = D6_B1; b2 = D6_B2; b3 = D6_B3; }
  else if (num == 7) { b0 = D7_B0; b1 = D7_B1; b2 = D7_B2; b3 = D7_B3; }
  else if (num == 8) { b0 = D8_B0; b1 = D8_B1; b2 = D8_B2; b3 = D8_B3; }
  else if (num == 9) { b0 = D9_B0; b1 = D9_B1; b2 = D9_B2; b3 = D9_B3; }
  buf[0] |= (b0 << rofs);
  buf[1] |= (b1 << rofs);
  buf[2] |= (b2 << rofs);
  buf[3] |= (b3 << rofs);
}

void Painter::setDot(uint8_t dot)
{
  uint8_t b0 = (1 << dot);
  buf[0] |= b0;
}

void Painter::setSeg(uint8_t digitIx, uint8_t segIx)
{
  if (segIx == 0) buf[3] |= (0x80 >> digitIx);
  else if (segIx == 1) buf[1] |= (0x08 >> digitIx);
  else if (segIx == 2) buf[3] |= (0x08 >> digitIx);
  else if (segIx == 3) buf[0] |= (0x80 >> digitIx);
  else if (segIx == 4) buf[1] |= (0x80 >> digitIx);
  else if (segIx == 5) buf[2] |= (0x80 >> digitIx);
  else if (segIx == 6) buf[2] |= (0x08 >> digitIx);
}
