/*******************************************************************************
MIT license 
Copyright 2016-2018 anxzhu (github.com/anxzhu)
https://github.com/anxzhu/segment-lcd-with-ht1621
Copyright 2018-2020 Valerio Nappi (github.com/valerionew) (changes)
https://github.com/valerionew/ht1621-7-seg
Copyright 2022 Gabor L Ugray (changes; reduced scope)
*******************************************************************************/

#ifndef HT1621_H
#define HT1621_H

#define HT1621_BUFSZ 6

class  HT1621
{
private:
	int csPin;
	int wrPin;
	int dataPin;

public:
	uint8_t buffer[HT1621_BUFSZ];

private:
	void wrData(uint8_t data, uint8_t cnt);
	void wrCmd(uint8_t cmd);
	void wrBufPos(uint8_t addr, uint8_t segData);

public:
	HT1621();
	void begin(uint8_t csPin, uint8_t wrPin, uint8_t dataPin);
	void setEnabled(bool enabled);
	void clearBuffer();
	void witeBuffer();
};

#endif
