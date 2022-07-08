#include <Arduino.h>
#include "painter.h"

const uint8_t digitMasks[10] = {
  SG_TP | SG_TL | SG_TR | SG_BL | SG_BR | SG_BM,
  SG_TR | SG_BR,
  SG_TP | SG_TR | SG_MD | SG_BL | SG_BM,
  SG_TP | SG_TR | SG_MD | SG_BR | SG_BM,
  SG_TL | SG_TR | SG_MD | SG_BR,
  SG_TP | SG_TL | SG_MD | SG_BR | SG_BM,
  SG_TP | SG_TL | SG_MD | SG_BL | SG_BR | SG_BM,
  SG_TP | SG_TR | SG_BR,
  SG_TP | SG_TL | SG_TR | SG_MD | SG_BL | SG_BR | SG_BM,
  SG_TP | SG_TL | SG_TR | SG_MD | SG_BR,
};

Painter::Painter(uint8_t *buf)
: buf(buf)
{
}

void Painter::setDigit(uint8_t digit, uint8_t val)
{
  if (val > 9)
    return;
  setSegs(digit, digitMasks[val]);
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

void Painter::setAllSegs(const uint8_t *pMasks)
{
  for (uint8_t i = 0; i < 4; ++i)
    setSegs(i, *(pMasks + i));
}

void Painter::setSegIx(uint8_t digit, uint8_t segIx)
{
  uint8_t mask = (1 << segIx);
  setSegs(digit, mask);
  // if (segIx == 0) buf[3] |= (0x80 >> digit);
  // else if (segIx == 1) buf[1] |= (0x08 >> digit);
  // else if (segIx == 2) buf[3] |= (0x08 >> digit);
  // else if (segIx == 3) buf[0] |= (0x80 >> digit);
  // else if (segIx == 4) buf[1] |= (0x80 >> digit);
  // else if (segIx == 5) buf[2] |= (0x80 >> digit);
  // else if (segIx == 6) buf[2] |= (0x08 >> digit);
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
