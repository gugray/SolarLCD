#include <Arduino.h>
#include "globals.h"
#include "loops.h"
#include "magic.h"

void setup()
{
  pinMode(PIN_LED, INPUT);
  ht1621.begin();
  currentLoopFun = startupLoop;
}

void loop()
{
  currentLoopFun();
}
