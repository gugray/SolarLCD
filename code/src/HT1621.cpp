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

// TODO: keep only binary form

#define  BIAS     0x52             //0b1000 0101 0010  1/3 duty 4 commons
#define  SYSDIS   0X00             //0b1000 0000 0000  关振系统荡器和LCD偏压发生器 ~ disable sys oscillator and LCD bias
#define  SYSEN    0X02             //0b1000 0000 0010  打开系统振荡器 ~ enable system oscillator
#define  LCDOFF   0X04             //0b1000 0000 0100  关LCD偏压 ~ disable LCD bias generator
#define  LCDON    0X06             //0b1000 0000 0110  打开LCD偏压 ~ enable LCD bias generator
#define  XTAL     0x28             //0b1000 0010 1000  外部接时钟 ~ external quartz crystal
#define  RC256    0X30             //0b1000 0011 0000  内部时钟 ~ internal RC oscillator
#define  WDTDIS1  0X0A             //0b1000 0000 1010  禁止看门狗 ~ disable watchdog timer

HT1621::HT1621()
{
	clearBuffer();
}

void HT1621::clearBuffer()
{
	memset(&buffer[0], 0, HT1621_BUFSZ);
}

void HT1621::begin(uint8_t csPin, uint8_t wrPin, uint8_t dataPin)
{
	// Store pins
	this->csPin = csPin;
	this->wrPin = wrPin;
	this->dataPin = dataPin;
	// Using them as output
	pinMode(csPin, OUTPUT);
	pinMode(wrPin, OUTPUT);
	pinMode(dataPin, OUTPUT);
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
		digitalWrite(csPin, LOW);
		wrData(0xa0, 3);
		wrData((addr << 2), 6);
		wrData(0, 8);
		digitalWrite(csPin, HIGH);
		addr = addr + 2;
	}
}

void HT1621::wrData(uint8_t data, uint8_t cnt)
{
	uint8_t i;
	for (i = 0; i < cnt; i++)
	{
		digitalWrite(wrPin, LOW);
		delayMicroseconds(4);
		if (data & 0x80)
		{
			digitalWrite(dataPin, HIGH);
		}
		else
		{
			digitalWrite(dataPin, LOW);
		}
		digitalWrite(wrPin, HIGH);
		delayMicroseconds(4);
		data <<= 1;
	}
}

void HT1621::setEnabled(bool enabled)
{
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

void HT1621::wrBufPos(uint8_t addr, uint8_t segData)
{
	addr <<= 2;
	digitalWrite(csPin, LOW);
	wrData(0xa0, 3);
	wrData(addr, 6);
	wrData(segData, 8);
	digitalWrite(csPin, HIGH);
}

void HT1621::wrCmd(uint8_t cmd)
{ // 100
	digitalWrite(csPin, LOW);
	wrData(0x80, 4);
	wrData(cmd, 8);
	digitalWrite(csPin, HIGH);
}

// takes the buffer and puts it straight into the driver
void HT1621::witeBuffer()
{
	// the buffer is backwards with respect to the lcd. could be improved
	wrBufPos(0, buffer[5]);
	wrBufPos(2, buffer[4]);
	wrBufPos(4, buffer[3]);
	wrBufPos(6, buffer[2]);
	wrBufPos(8, buffer[1]);
	wrBufPos(10, buffer[0]);
}
