#include <Arduino.h>
#include "animations.h"
#include "globals.h"

#define STOP_AFTER_N_SECONDS 12
#define ANIM_PERIOD 4

#define NOISE_LEN 8 
const PROGMEM uint8_t noise1[NOISE_LEN] = { 7, 69, 98, 87, 37, 88, 93, 110 }; 
const PROGMEM uint8_t noise2[NOISE_LEN] = { 7, 94, 123, 87, 37, 63, 119, 110 }; 
const PROGMEM uint8_t noise3[NOISE_LEN] = { 2, 69, 123, 56, 36, 37, 123, 106 }; 

// const PROGMEM uint8_t noise1[NOISE_LEN] = {118, 88, 123, 94, 52, 81, 0, 43 }; 
// const PROGMEM uint8_t noise2[NOISE_LEN] = {94, 94, 123, 118, 52, 106, 0, 68 }; 

void drawEqualizerBar(const uint8_t *buf, uint8_t segMask)
{
  uint8_t nVals = NOISE_LEN * 3;
  uint8_t ix = (as.counter / ANIM_PERIOD) % nVals;
  uint8_t bufPos = ix / 3;
  uint8_t ofs = ix % 3;
  uint8_t cval = pgm_read_byte_near(buf + bufPos);
  // uint8_t cval = buf[bufPos];
  uint8_t val;
  uint8_t val0 = cval / 25;
  if (ofs == 0) val = val0;
  else
  {
    uint8_t val1 = (cval - val0 * 25) / 5;
    if (ofs == 1) val = val1;
    else
    {
      uint8_t val2 = cval - val0 * 25 - val1 * 5;
      val = val2;
    }
  }

  if (val > 0) painter.setSegs(0, segMask);
  if (val > 1) painter.setSegs(1, segMask);
  if (val > 2) painter.setSegs(2, segMask);
  if (val > 3) painter.setSegs(3, segMask);
}

bool animEqualizer(bool fast)
{
  // We act on every 4th cycle, i.e., every 128msec
  // That'a frame rate of about 8
  if ((as.counter++ % ANIM_PERIOD) != 0)
    return false;

  ht1621.clearBuffer();
  drawEqualizerBar(noise1, SG_TP);
  drawEqualizerBar(noise2, SG_MD);
  drawEqualizerBar(noise3, SG_BM);
  ht1621.wrBuffer();

  // Do this for about N seconds
  return as.counter >= STOP_AFTER_N_SECONDS * 32;
}
