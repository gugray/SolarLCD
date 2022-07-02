#include <Arduino.h>
#include "animations.h"
#include "euclidean.h"
#include "globals.h"

#define FLAG_SETUP 1

/* AnimState usage
   counter: frame count
   a: current count in 16-beat bar
   b: scaler (beat at every 'scaler'-th frame)
   c: bars played
   d:
   mem:
      [0..3]: Current value per track (0, 1, 2)
      [4..27]: Track definitions, 6 bytes per track
               Each 6 bytes: rhythm 1 and optional rhythm 2
               [nBeats, offset, volume], [nBeats, offset, volume]
*/

void setupEuclideanO()
{
  as.b = random(5, 11);

  uint8_t nVoices = random(4, 6);
  for (uint8_t i = 0; i < nVoices; ++i)
  {
    uint8_t trackIx = i % 4;
    uint8_t nBeats = random(1, 6);
    uint8_t *ptr = as.mem + 4 + 6 * trackIx;
    if (*ptr != 0)
      ptr += 3;
    *ptr = nBeats;
    *(ptr + 1) = random(0, 16 / nBeats); // offset
    *(ptr + 2) = random(1, 3);           // volume
  }
}

void beatEuclideanO()
{
  for (uint8_t i = 0; i < 4; ++i)
  {
    uint8_t val = 0;
    uint8_t *ptr = as.mem + 4 + 6 * i;
    uint8_t nBeats = *ptr;
    uint8_t offset = *(ptr + 1);
    uint8_t volume = *(ptr + 2);
    if (isOnBeat(nBeats, offset, as.a))
      val += volume;
    ptr += 3;
    nBeats = *ptr;
    if (nBeats != 0)
    {
      offset = *(ptr + 1);
      volume = *(ptr + 2);
      if (isOnBeat(nBeats, offset, as.a))
        val += volume;
    }
    if (val > 2)
      val = 2;
    ptr = as.mem + i;
    if (*ptr > 0)
      *ptr -= 1;
    val += *ptr;
    if (val > 2)
      val = 2;
    *ptr = val;
  }
  ++as.a;
  if (as.a == 16)
  {
    as.a = 0;
    ++as.c;
  }
}

bool animEuclideanO()
{
  // Setup once
  if (!as.getFlag(FLAG_SETUP))
  {
    setupEuclideanO();
    as.setFlag(FLAG_SETUP, true);
  }

  if ((as.counter++ % as.b) != 0)
    return false;

  // Update value of each track
  beatEuclideanO();

  // If we're done, return here, before flashing up last screen for 1 frame
  // Play 8 bars
  if (as.c >= 8)
    return true;

  // Visualize
  ht1621.clearBuffer();
  uint8_t *ptr;
  uint8_t i;
  for (ptr = as.mem, i = 0; i < 4; ++i, ++ptr)
  {
    painter.setSegs(i, SG_BM);
    if (*ptr >= 1)
      painter.setSegs(i, SG_MD | SG_BL | SG_BR);
    if (*ptr >= 2)
      painter.setSegs(i, SG_TP | SG_TL | SG_TR);
  }
  ht1621.wrBuffer();

  return false;
}
