/*******************************************************************************
MIT license
Copyright 2016-2018 anxzhu (github.com/anxzhu)
https://github.com/anxzhu/segment-lcd-with-ht1621
Copyright 2018-2020 Valerio Nappi (github.com/valerionew) (changes)
https://github.com/valerionew/ht1621-7-seg
Copyright 2022 Gabor L Ugray (changes; reduced scope)
*******************************************************************************/

/******** Segment-to-buffer map *********************************************************************

         3:80                      3:40                      3:20                      3:10
     ░░░░░░░░░░░             	 ░░░░░░░░░░░               ░░░░░░░░░░░     0:01      ░░░░░░░░░░░
    ░     A0    ░             ░     B0    ░             ░     C0    ░      ░      ░     D0    ░
    ░           ░             ░           ░             ░           ░     X3      ░           ░
  A1░1:08     A2░3:08       B1░1:04     B2░3:04       C1░1:02     C2░3:02       D1░1:01     D2░3:01
    ░           ░         	  ░           ░          	  ░           ░          	  ░           ░
    ░    0:80   ░         		░    0:40   ░    0:02  		░    0:20   ░          		░    0:10   ░
     ░░░░░░░░░░░          		 ░░░░░░░░░░░       ░   		 ░░░░░░░░░░░           		 ░░░░░░░░░░░
    ░     A3    ░         		░     B3    ░     X2   		░     C3    ░          		░     D3    ░
    ░           ░         		░           ░          		░           ░          		░           ░
  A4░1:80     A5░2:80     	B4░1:40     B5░2:40      	C4░1:20     C5░2:20      	D4░1:10     D5░2:10
    ░           ░         	  ░           ░          	  ░           ░          	  ░           ░
    ░    2:08   ░    0:08 		░    2:04   ░    0:04  		░    2:02   ░          		░    2:01   ░
     ░░░░░░░░░░░       ░  		 ░░░░░░░░░░░       ░   		 ░░░░░░░░░░░           		 ░░░░░░░░░░░
          A6          X0  		      B6          X1   		      C6               		      D6

Digits on the display are A B C D. Segments go from 0 thru 6 in each digit.
First decimal dot is X0; second is X1; middle dot is X2; degree symbol is X3
Position N:NN means bit 0xNN in the Nth byte (0 thru 3)

*****************************************************************************************************/

#ifndef HT1621_H
#define HT1621_H

#define HT1621_BUFSZ 4

class HT1621
{
private:
  bool enabled;

public:
  uint8_t buffer[HT1621_BUFSZ];

private:
  void wrData(uint8_t data, uint8_t cnt);
  void wrCmd(uint8_t cmd);

public:
  HT1621();
  void begin();
  bool isEnabled();
  void setEnabled(bool enabled);
  void clearBuffer();
  void wrBufPos(uint8_t ix);
  void wrBuffer();
};

#endif
