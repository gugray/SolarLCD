#include <Arduino.h>
#include "globals.h"
#include "mode_loops.h"
#include "magic.h"

void setup()
{
  ht1621.begin();
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);
  currentLoopFun = startupLoop;
}

void loop()
{
  currentLoopFun();
}
