/*******************************************************************************
MIT license
Copyright 2016-2018 anxzhu (github.com/anxzhu)
https://github.com/anxzhu/segment-lcd-with-ht1621
Copyright 2018-2020 Valerio Nappi (github.com/valerionew) (changes)
https://github.com/valerionew/ht1621-7-seg
Copyright 2022 Gabor L Ugray (changes; reduced scope)
*******************************************************************************/

#include <Arduino.h>
#include "HT1621.h"
#include "magic.h"

#define BIAS 0b01010010    // 1/3 duty 4 commons
#define SYSDIS 0b00000000  // 关振系统荡器和LCD偏压发生器 ~ disable sys oscillator and LCD bias
#define SYSEN 0b00000010   // 打开系统振荡器 ~ enable system oscillator
#define LCDOFF 0b00000100  // 关LCD偏压 ~ disable LCD bias generator
#define LCDON 0b00000110   // 打开LCD偏压 ~ enable LCD bias generator
#define XTAL 0b00101000    // 外部接时钟 ~ external quartz crystal
#define RC256 0b00110000   // 内部时钟 ~ internal RC oscillator
#define WDTDIS1 0b00001010 // 禁止看门狗 ~ disable watchdog timer

HT1621::HT1621()
{
  enabled = false;
  clearBuffer();
}

bool HT1621::isEnabled()
{
  return enabled;
}

void HT1621::clearBuffer()
{
  memset(&buffer[0], 0, HT1621_BUFSZ);
}

void HT1621::begin()
{
  // Using them as output
  pinMode(PIN_CS, OUTPUT);
  pinMode(PIN_WR, OUTPUT);
  pinMode(PIN_DATA, OUTPUT);
  // Init sequence
  wrCmd(BIAS);
  wrCmd(RC256);
  wrCmd(SYSDIS);
  wrCmd(WDTDIS1);
  wrCmd(SYSEN);
  wrCmd(LCDON);
  // Clear sequence
  uint8_t i, addr;
  for (i = 0, addr = 0; i < 16; i++)
  {
    digitalWrite(PIN_CS, LOW);
    wrData(0xa0, 3);
    wrData((addr << 2), 6);
    wrData(0, 8);
    digitalWrite(PIN_CS, HIGH);
    addr = addr + 2;
  }
  enabled = true;
}

void HT1621::wrData(uint8_t data, uint8_t cnt)
{
  uint8_t i;
  for (i = 0; i < cnt; i++)
  {
    digitalWrite(PIN_WR, LOW);
    delayMicroseconds(4);
    digitalWrite(PIN_DATA, (data & 0x80) ? HIGH : LOW);
    digitalWrite(PIN_WR, HIGH);
    delayMicroseconds(4);
    data <<= 1;
  }
}

void HT1621::setEnabled(bool enabled)
{
  if (this->enabled == enabled)
    return;
  this->enabled = enabled;

  if (enabled)
  {
    wrCmd(SYSEN); // System oscillator
    wrCmd(LCDON); // LCD bias generator
  }
  else
  {
    // This turns off both system oscillator and LCD bias generator
    wrCmd(SYSDIS);
  }
}

void HT1621::wrCmd(uint8_t cmd)
{
  digitalWrite(PIN_CS, LOW);
  wrData(0x80, 4);
  wrData(cmd, 8);
  digitalWrite(PIN_CS, HIGH);
}

void HT1621::wrBufPos(uint8_t ix)
{
  uint8_t segData = buffer[ix];
  uint8_t addr = 3 - ix;
  addr <<= 3;
  digitalWrite(PIN_CS, LOW);
  wrData(0xa0, 3);
  wrData(addr, 6);
  wrData(segData, 8);
  digitalWrite(PIN_CS, HIGH);
}

// takes the buffer and puts it straight into the driver
void HT1621::wrBuffer()
{
  wrBufPos(0);
  wrBufPos(1);
  wrBufPos(2);
  wrBufPos(3);
}
