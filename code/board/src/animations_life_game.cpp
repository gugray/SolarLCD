#include <Arduino.h>
#include "animations.h"
#include "globals.h"

#define END_AT_N_CAOS 5
#define END_AT_N_SECONDS 120
#define CHAOS_AFTER_LOOP_STEPS 16
#define ANIM_PERIOD 16
#define N_LOOKBACK 4
#define FLAG_SETUP 1
#define FLAG_CAOS 2

/* AnimState usage
   counter: frame count
   a:
      If flashing CAOS: Frames left to flash CAOS
      Normally: index of oldest lookback (4 bytes) in mem
   b: Number of lookbacks stored
   c: Steps we've spent in a loop
   d: Number of times CAOS has been invoked
   mem:
      While flashing CAOS: first 4 bytes are last simulation buffer to restore
      Normally: Up to N_LOOKBACK lookback copies, used as a ring buffer of 4-byte chunks
*/

void setupLifeGame()
{
  ht1621.clearBuffer();
  // 1 or 2 start segments, but we want more cases of 2
  uint8_t nStartSegs = random(3) + 1;
  if (nStartSegs > 2)
    nStartSegs = 2;
  for (uint8_t i = 0; i < nStartSegs; ++i)
  {
    while (true)
    {
      uint8_t ix = random(28);
      uint8_t dix = ix / 7;
      uint8_t six = ix % 7;
      if (painter.getSegIx(dix, six))
        continue;
      painter.setSegIx(dix, six);
      break;
    }
  }
}

void setChaos(bool active)
{
  if (active)
  {
    // Flip a random segment
    uint8_t ix = random(28);
    uint8_t dix = ix / 7;
    uint8_t six = ix % 7;
    if (painter.getSegIx(dix, six))
      painter.clearSegIx(dix, six);
    else
      painter.setSegIx(dix, six);
    // Make pre-CAOS-flash copy
    memcpy(as.mem, ht1621.buffer, 4);
    // Kick off CAOS flash
    as.a = 48;
    as.setFlag(FLAG_CAOS, true);
  }
  else
  {
    memcpy(ht1621.buffer, as.mem, 4);
    memset(as.mem, 0, AS_MEM_SZ);
    as.a = 0;
    as.b = 0;
    as.c = 0;
    ++as.d; // Increase CAOS count once animation is finished
    as.setFlag(FLAG_CAOS, false);
  }
}

uint8_t dmod(uint8_t digitIx)
{
  if (digitIx == 255)
    return 3;
  else if (digitIx >= 4)
    return digitIx -= 4;
  else
    return digitIx;
}

uint8_t countAliveNeighbors(uint8_t digitIx, uint8_t segIx)
{
  if (segIx == 0)
    return painter.getSegIx(digitIx, 6) + painter.getSegIx(digitIx, 1) + painter.getSegIx(digitIx, 2);
  else if (segIx == 1)
    return painter.getSegIx(dmod(digitIx - 1), 2) + painter.getSegIx(digitIx, 0) + painter.getSegIx(digitIx, 3) + painter.getSegIx(digitIx, 4);
  else if (segIx == 2)
    return painter.getSegIx(digitIx, 0) + painter.getSegIx(digitIx, 3) + painter.getSegIx(digitIx, 5) + painter.getSegIx(dmod(digitIx + 1), 1);
  else if (segIx == 3)
    return painter.getSegIx(digitIx, 1) + painter.getSegIx(digitIx, 2) + painter.getSegIx(digitIx, 4) + painter.getSegIx(digitIx, 5);
  else if (segIx == 4)
    return painter.getSegIx(dmod(digitIx - 1), 5) + painter.getSegIx(digitIx, 1) + painter.getSegIx(digitIx, 3) + painter.getSegIx(digitIx, 6);
  else if (segIx == 5)
    return painter.getSegIx(digitIx, 3) + painter.getSegIx(digitIx, 2) + painter.getSegIx(digitIx, 6) + painter.getSegIx(dmod(digitIx + 1), 4);
  else
    return painter.getSegIx(digitIx, 4) + painter.getSegIx(digitIx, 5) + painter.getSegIx(digitIx, 0);
}

void archiveCurrentScreen()
{
  uint8_t ix;
  if (as.b < N_LOOKBACK)
  {
    ix = as.b; // as.a must be 0 here
    ++as.b;
  }
  else
  {
    ++as.a;
    if (as.a == N_LOOKBACK)
      as.a = 0;
    ix = (as.a + as.b) % N_LOOKBACK;
  }
  uint8_t *ptr = as.mem + 4 * ix;
  memcpy(ptr, ht1621.buffer, 4);
}

uint8_t distFromLast(uint8_t *nbuf)
{
  if (as.b == 0)
    return 255;
  uint8_t ix = (as.a + as.b - 1) % 4;
  for (uint8_t i = 0; i < N_LOOKBACK; ++i)
  {
    uint8_t *ptr = as.mem + 4 * ix;
    if (memcmp(nbuf, ptr, 4) == 0)
      return i;
    if (ix == 0)
      ix = N_LOOKBACK - 1;
    else
      ix -= 1;
  }
  return 0xff;
}

void stepLifeGame()
{
  // New screen will be in nbuf, built via npainter
  uint8_t nbuf[4];
  Painter npainter(nbuf);
  for (uint8_t i = 0; i < 4; ++i)
    nbuf[i] = 0;
  for (uint8_t i = 0; i < 4; ++i)
  {
    for (uint8_t j = 0; j < 7; ++j)
    {
      bool alive = painter.getSegIx(i, j) != 0;
      uint8_t aliveNeighbors = countAliveNeighbors(i, j);
      bool aliveAfter;
      // Rules of the game!
      // 1 1 2: oscillates quickly
      // 2 1 2: long irregular games
      // 2 1 3: oscillates quickly
      // 2 1 4: long games, interesting oscillations <=== LIKE
      if (alive)
        aliveAfter = aliveNeighbors == 2;
      else
      {
        if (aliveNeighbors < 1)
          aliveAfter = false;
        else if (aliveNeighbors > 4)
          aliveAfter = false;
        else
          aliveAfter = true;
      }
      if (aliveAfter)
        npainter.setSegIx(i, j);
    }
  }
  // Seen this screen before?
  uint8_t dist = distFromLast(nbuf);
  if (dist != 255)
    ++as.c;

  // Archive current screen; refresh display
  archiveCurrentScreen();
  memcpy(ht1621.buffer, nbuf, 4);
}

bool animLifeGame()
{
  // Setup once
  if (!as.getFlag(FLAG_SETUP))
  {
    setupLifeGame();
    as.setFlag(FLAG_SETUP, true);
    ht1621.wrBuffer();
    ++as.counter;
    return false;
  }

  // Blinking CAOS?
  if (as.getFlag(FLAG_CAOS))
  {
    ht1621.clearBuffer();
    if ((as.a % 8) < 4)
    {
      painter.setSegs(0, SG_TP | SG_TL | SG_BL | SG_BM);
      painter.setSegs(1, SG_TP | SG_TL | SG_TR | SG_MD | SG_BL | SG_BR);
      painter.setSegs(2, SG_TP | SG_TL | SG_TR | SG_BL | SG_BR | SG_BM);
      painter.setSegs(3, SG_TP | SG_TL | SG_MD | SG_BR | SG_BM);
    }
    ht1621.wrBuffer();
    --as.a;
    if (as.a > 0)
    {
      ++as.counter;
      return false;
    }
    setChaos(false);
    as.counter -= (as.counter % ANIM_PERIOD);
    --as.counter;
  }

  // We act on every 4th frame, i.e., every 128msec
  // That'a visual frame rate of about 2
  if ((as.counter++ % ANIM_PERIOD) != 0)
    return false;

  stepLifeGame();
  ht1621.wrBuffer();

  // If we're in a cycle for long, call in the chaos monkey
  if (as.c > CHAOS_AFTER_LOOP_STEPS)
    setChaos(true);

  // If we've flashed CAOS X times, stop
  if (as.d >= END_AT_N_CAOS)
    return true;
  // If we've been going X seconds and we're currently not in CAOS, stop
  if (as.counter >= 32 * END_AT_N_SECONDS && !as.getFlag(FLAG_CAOS))
    return true;
  return false;
}
