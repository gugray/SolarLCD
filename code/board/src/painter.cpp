#include <Arduino.h>
#include "painter.h"

Painter::Painter(uint8_t *buf)
: buf(buf)
{
}

void Painter::setDigit(uint8_t digit, uint8_t val)
{
  uint8_t mask;
  switch (val)
  {
  case 0:
    mask = SG_TP | SG_TL | SG_TR | SG_BL | SG_BR | SG_BM;
    break;
  case 1:
    mask = SG_TR | SG_BR;
    break;
  case 2:
    mask = SG_TP | SG_TR | SG_MD | SG_BL | SG_BM;
    break;
  case 3:
    mask = SG_TP | SG_TR | SG_MD | SG_BR | SG_BM;
    break;
  case 4:
    mask = SG_TL | SG_TR | SG_MD | SG_BR;
    break;
  case 5:
    mask = SG_TP | SG_TL | SG_MD | SG_BR | SG_BM;
    break;
  case 6:
    mask = SG_TP | SG_TL | SG_MD | SG_BL | SG_BR | SG_BM;
    break;
  case 7:
    mask = SG_TP | SG_TR | SG_BR;
    break;
  case 8:
    mask = SG_TP | SG_TL | SG_TR | SG_MD | SG_BL | SG_BR | SG_BM;
    break;
  case 9:
    mask = SG_TP | SG_TL | SG_TR | SG_MD | SG_BR;
    break;
  case '-':
    mask = SG_MD;
    break;
  case 'E':
    mask = SG_TP | SG_TL | SG_MD | SG_BL | SG_BM;
    break;
  case 'H':
    mask = SG_TL | SG_TR | SG_MD | SG_BL | SG_BR;
    break;
  case 'L':
    mask = SG_TL | SG_BL | SG_BM;
    break;
  case 'r':
    mask = SG_MD | SG_BL;
    break;
  case 'u':
    mask = SG_BL | SG_BR | SG_BM;
    break;
  default:
    return;
  }
  setSegs(digit, mask);
}

void Painter::setDots(uint8_t mask)
{
  if (mask & DT_LDEC) buf[0] |= 0x08;
  if (mask & DT_MDEC) buf[0] |= 0x04;
  if (mask & DT_MDOT) buf[0] |= 0x02;
  if (mask & DT_DEGR) buf[0] |= 0x01;
}

void Painter::setSegs(uint8_t digit, uint8_t mask)
{
  if (mask & SG_TP) buf[3] |= (0x80 >> digit);
  if (mask & SG_TL) buf[1] |= (0x08 >> digit);
  if (mask & SG_TR) buf[3] |= (0x08 >> digit);
  if (mask & SG_MD) buf[0] |= (0x80 >> digit);
  if (mask & SG_BL) buf[1] |= (0x80 >> digit);
  if (mask & SG_BR) buf[2] |= (0x80 >> digit);
  if (mask & SG_BM) buf[2] |= (0x08 >> digit);
}

void Painter::setSegIx(uint8_t digit, uint8_t segIx)
{
  if (segIx == 0) buf[3] |= (0x80 >> digit);
  else if (segIx == 1) buf[1] |= (0x08 >> digit);
  else if (segIx == 2) buf[3] |= (0x08 >> digit);
  else if (segIx == 3) buf[0] |= (0x80 >> digit);
  else if (segIx == 4) buf[1] |= (0x80 >> digit);
  else if (segIx == 5) buf[2] |= (0x80 >> digit);
  else if (segIx == 6) buf[2] |= (0x08 >> digit);
}

void Painter::clearSegIx(uint8_t digit, uint8_t segIx)
{
  if (segIx == 0) buf[3] &= 0xff - (0x80 >> digit);
  else if (segIx == 1) buf[1] &= 0xff - (0x08 >> digit);
  else if (segIx == 2) buf[3] &= 0xff - (0x08 >> digit);
  else if (segIx == 3) buf[0] &= 0xff - (0x80 >> digit);
  else if (segIx == 4) buf[1] &= 0xff - (0x80 >> digit);
  else if (segIx == 5) buf[2] &= 0xff - (0x80 >> digit);
  else if (segIx == 6) buf[2] &= 0xff - (0x08 >> digit);
}

uint8_t Painter::getSegIx(uint8_t digit, uint8_t segIx)
{
  uint8_t val;
  if (segIx == 0) val = buf[3] & (0x80 >> digit);
  else if (segIx == 1) val = buf[1] & (0x08 >> digit);
  else if (segIx == 2) val = buf[3] & (0x08 >> digit);
  else if (segIx == 3) val = buf[0] & (0x80 >> digit);
  else if (segIx == 4) val = buf[1] & (0x80 >> digit);
  else if (segIx == 5) val = buf[2] & (0x80 >> digit);
  else if (segIx == 6) val = buf[2] & (0x08 >> digit);
  return val == 0 ? 0 : 1;
}

void Painter::setImage(Images img)
{
  switch (img)
  {
  case frog:
    setSegs(0, SG_BL | SG_BM);
    setSegs(1, SG_TP | SG_TL | SG_TR | SG_MD | SG_BM);
    setSegs(2, SG_TP | SG_TL | SG_TR | SG_MD | SG_BM);
    setSegs(3, SG_BR | SG_BM);
    break;
  case helo:
    setDigit(0, 'H');
    setDigit(1, 'E');
    setDigit(2, 'L');
    setDigit(3, 0);
    break;
  default:
    return;
  }
}
