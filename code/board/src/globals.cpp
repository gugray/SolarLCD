#include <Arduino.h>
#include "globals.h"

HT1621 ht1621;
Painter painter(ht1621.buffer);

// volatile uint32_t tcnt = 0;
