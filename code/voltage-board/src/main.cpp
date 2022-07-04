#include <Arduino.h>
#include "HT1621.h"
#include "painter.h"
#include "OneWire.h"

#define PIN_LED_TEMP 2

#define BUFLEN 4
uint8_t buf[BUFLEN];

HT1621 ht1621;
Painter painter(ht1621.buffer);
OneWire ds(PIN_LED_TEMP);

// defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

void setup()
{
  pinMode(PIN_LED_TEMP, INPUT);

  // ADC prescaler: 128
  sbi(ADCSRA,ADPS2);
  sbi(ADCSRA,ADPS1);
  cbi(ADCSRA,ADPS0);


  ht1621.begin();
  ht1621.clearBuffer();
  painter.setImage(Painter::Images::helo);
  ht1621.wrBuffer();

  analogReference(INTERNAL);

  // delay(2000);
  Serial.begin(9600);
}

int64_t measure()
{
  int64_t val = analogRead(A0);
  int64_t voltage = 570 * val / 1024;
  return voltage;
}

void voltageLoop()
{
  int64_t sum = 0;
  int64_t count = 8;
  for (uint8_t i = 0; i < count; ++i)
  {
    sum += measure();
    delay(20);
  }
  int64_t voltage = sum / count;

  int d2, d1, d0;
  d2 = (int)(voltage % 10);
  voltage /= 10;
  d1 = (int)(voltage % 10);
  voltage /= 10;
  d0 = (int)(voltage % 10);

  Serial.print(d0);
  Serial.print(d1);
  Serial.print(d2);
  Serial.println();

  ht1621.clearBuffer();
  painter.setDigit(2, d2);
  painter.setDigit(1, d1);
  painter.setDigit(0, d0);
  painter.setDots(DT_LDEC);
  ht1621.wrBuffer();
  Serial.println();

  delay(1000 - count * 20);
}

void loop()
{
  byte data[9];

  ds.reset();
  ds.skip();
  ds.write(0x44); // start conversion

  delay(1000); // maybe 750ms is enough, maybe not

  ds.reset();
  ds.skip();
  ds.write(0xBE); // Read Scratchpad
  for (uint8_t i = 0; i < 9; i++)
    data[i] = ds.read();

  // Verify CRC if we so please

  ht1621.clearBuffer();

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
  temp = temp * 10 / 16; // Celsius times 10
  // celsius = (float)raw / 16.0;
  // fahrenheit = celsius * 1.8 + 32.0;

  int d2, d1, d0;
  d2 = (int)(temp % 10);
  temp /= 10;
  d1 = (int)(temp % 10);
  temp /= 10;
  d0 = (int)(temp % 10);

  painter.setDigit(2, d2);
  painter.setDigit(1, d1);
  painter.setDigit(0, d0);
  painter.setDots(DT_MDEC | DT_DEGR);
  // painter.setSegs(3, SG_TP | SG_TL | SG_BL | SG_BM);
  ht1621.wrBuffer();

  // delay(500);
  // pinMode(PIN_LED_TEMP, OUTPUT);
  // digitalWrite(PIN_LED_TEMP, LOW);
  // delay(16);
  // pinMode(PIN_LED_TEMP, INPUT);
}
