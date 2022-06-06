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

void drawVoltage(int num)
{
  int val = num;
  int n = val % 10;
  painter.setDigit(n, 0);
  val /= 10;
  n = val % 10;
  if (val != 0) painter.setDigit(n, 1);
  val /= 10;
  n = val % 10;
  if (val != 0) painter.setDigit(n, 2);
  val /= 10;
  n = val % 10;
  if (val != 0) painter.setDigit(n, 3);

  painter.setDot(2);
}

bool lcdOn = true;
bool dotShown = false;

void loop()
{

  int16_t vcc = get_vcc();
  ht1621.clearBuffer();

  // If voltage is below 2.5: we show display for 0.5 sec, then sleep for 4 sec
  if (vcc < 250)
  {
    if (!lcdOn) ht1621.setEnabled(true);
    drawVoltage(vcc);
    ht1621.witeBuffer();
    sleep(5);
    ht1621.setEnabled(false);
    lcdOn = false;
    sleep(8);
  }
  // For higher voltage: LCD always on; show temp; toggle middle dot
  // Sleep for 0.5 sec
  else
  {
    if (!lcdOn) ht1621.setEnabled(true);
    lcdOn = true;
    drawVoltage(vcc);
    if (!dotShown) painter.setDot(1);
    dotShown = !dotShown;
    ht1621.witeBuffer();
    sleep(5);
  }
}
