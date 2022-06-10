#include <Arduino.h>
#include "mode_loops.h"
#include "globals.h"
#include "sleep.h"
#include "vcc.h"
#include "magic.h"

LoopFun currentLoopFun;
bool lcdOn = true;
bool dotShown = false;


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


void lowVoltageLoop()
{
  int16_t vcc = get_vcc();
  ht1621.clearBuffer();

  // If voltage is now above threshold: change mode
  if (vcc > BLINK_THRESHOLD_VCC)
  {
    currentLoopFun = goodVoltageLoop;
    return;
  }

  // If voltage tooooo low, no LCD
  if (vcc >= LCD_THRESHOLD_VCC)
  {
    if (!lcdOn) ht1621.setEnabled(true);
    drawVoltage(vcc);
    ht1621.wrBuffer();
    sleep(5); // 500 msec
    ht1621.setEnabled(false);
    lcdOn = false;
  }
  if (vcc >= LCD_THRESHOLD_VCC) sleep(8); // 4 sec
  else sleep(9); // 8 sec
  digitalWrite(PIN_LED, HIGH);
  sleep(0); // 16 msec
  digitalWrite(PIN_LED, LOW);
  sleep(5); // 500 msec
}

void goodVoltageLoop()
{
  int16_t vcc = get_vcc();
  ht1621.clearBuffer();

  // If voltage is now below threshold: change mode
  if (vcc <= BLINK_THRESHOLD_VCC)
  {
    currentLoopFun = lowVoltageLoop;
    return;
  }

  if (!lcdOn) ht1621.setEnabled(true);
  lcdOn = true;
  drawVoltage(vcc);
  if (!dotShown) painter.setDot(1);
  dotShown = !dotShown;
  ht1621.wrBuffer();
  sleep(5); // 500 msec
}

void smileyLoop()
{
  ht1621.clearBuffer();
  // Left eye
  painter.setSeg(1, 0);
  painter.setSeg(1, 1);
  painter.setSeg(1, 2);
  painter.setSeg(1, 3);
  // Right eye
  painter.setSeg(2, 0);
  painter.setSeg(2, 1);
  painter.setSeg(2, 2);
  painter.setSeg(2, 3);
  // Mouth
  painter.setSeg(0, 4);
  painter.setSeg(0, 6);
  painter.setSeg(1, 6);
  painter.setSeg(2, 6);
  painter.setSeg(3, 6);
  painter.setSeg(3, 5);

  ht1621.wrBuffer();
  sleep(5); // 500 msec
}
