#include <Arduino.h>
#include <OneWire.h>
#include "temp.h"
#include "magic.h"


volatile int16_t tempC = 0;
volatile int16_t tempF = 0;
OneWire ds(PIN_LED);
byte data[9];

void startTempConversion()
{
  ds.reset();
  ds.skip();
  ds.write(0x44); // start conversion
}

void readTemp()
{
  ds.reset();
  ds.skip();
  ds.write(0xBE); // Read Scratchpad
  for (uint8_t i = 0; i < 9; i++)
    data[i] = ds.read();

  // Convert the data to actual temperature
  int16_t raw = (data[1] << 8) | data[0]; // default is 12 bit resolution
  uint8_t cfg = (data[4] & 0x60);
  // at lower res, the low bits are undefined, so let's zero them
  if (cfg == 0x00)
    raw = raw & ~7;  // 9 bit resolution
  else if (cfg == 0x20) 
    raw = raw & ~3; // 10 bit res
  else if (cfg == 0x40) 
    raw = raw & ~1; // 11 bit res

  int32_t temp = raw;
  tempC = temp * 5 / 8; // Celsius times 10
  tempF = 32 + temp * 9 / 80;
  // celsius = (float)raw / 16.0;
  // fahrenheit = celsius * 1.8 + 32.0;
}
