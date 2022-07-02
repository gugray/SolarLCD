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
      [0..3]: Current value per track (0 or 1)
      [4..11]: Track definitions, 2 bytes per track
               [nBeats, offset]
*/

void setupEuclideanU()
{
  as.b = random(6, 11);

  for (uint8_t trackIx = 0; trackIx < 4; ++trackIx)
  {
    uint8_t nBeats = random(4, 7);
    uint8_t *ptr = as.mem + 4 + 2 * trackIx;
    *ptr = nBeats;
    *(ptr + 1) = random(0, 16 / nBeats); // offset
  }
}

void beatEuclideanU()
{
  for (uint8_t i = 0; i < 4; ++i)
  {
    uint8_t *ptr = as.mem + 4 + 2 * i;
    uint8_t nBeats = *ptr;
    uint8_t offset = *(ptr + 1);
    if (isOnBeat(nBeats, offset, as.a))
      *(as.mem + i) = 1;
    else
      *(as.mem + i) = 0;
  }
  ++as.a;
  if (as.a == 16)
  {
    as.a = 0;
    ++as.c;
  }
}

bool animEuclideanU()
{
  // Setup once
  if (!as.getFlag(FLAG_SETUP))
  {
    setupEuclideanU();
    as.setFlag(FLAG_SETUP, true);
  }

  if ((as.counter++ % as.b) != 0)
    return false;

  // Update value of each track
  beatEuclideanU();

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
    if (*ptr != 0)
      painter.setSegs(i, SG_BL | SG_BR | SG_BM);
    uint8_t eighths = (as.a / 2) % 8;
    bool left = (eighths % 2) == 0;
    painter.setSegIx(eighths / 2, 0);
    painter.setSegIx(eighths / 2, left ? 1 : 2);
  }
  ht1621.wrBuffer();

  return false;
}
