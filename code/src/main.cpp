#include <Arduino.h>
#include "HT1621.h"
#include "painter.h"
#include "vcc.h"
#include "sleep.h"

HT1621 ht1621;
Painter painter(&ht1621.buffer[2]);

// ATTiny pinout
#define PIN_CS    4
#define PIN_WR    2
#define PIN_DATA  1

void setup()
{
  ht1621.begin(PIN_CS, PIN_WR, PIN_DATA);
}

// void drawDigit(int num, int rofs)
// {
//   for (int i = 0; i < 4; ++i)
//   {
//     ht1621.buffer[i + 2] |= (digits[num * 4 + i] << rofs);
//   }
// }

void drawNumber(int num)
{
  for (int i = 0; i < 4; ++i) ht1621.buffer[i+2] = 0;

  int val = num;
  int n = val % 10;
  painter.drawDigit(n, 0);
  val /= 10;
  n = val % 10;
  if (val != 0) painter.drawDigit(n, 1);
  val /= 10;
  n = val % 10;
  if (val != 0) painter.drawDigit(n, 2);
  val /= 10;
  n = val % 10;
  if (val != 0) painter.drawDigit(n, 3);

  ht1621.witeBuffer();
}

void loop()
{

  int16_t vcc = get_vcc();
  drawNumber(vcc);

  // delay(500);
  sleep(5);
}
