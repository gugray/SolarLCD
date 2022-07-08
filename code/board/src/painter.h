#ifndef PAINTER_H
#define PAINTER_H

#define SG_TP   0x01  // Top horizontal
#define SG_TL   0x02  // Top left
#define SG_TR   0x04  // Top right
#define SG_MD   0x08  // Middle horizontal
#define SG_BL   0x10  // Bottom left
#define SG_BR   0x20  // Bottom right
#define SG_BM   0x40  // Bottom horizontal

#define DT_LDEC 0x80  // Left decimal
#define DT_MDEC 0x40  // Mid decimal
#define DT_MDOT 0x20  // Mid dot
#define DT_DEGR 0x10  // Degree sign

class Painter
{
private:
  uint8_t *buf;

public:

  // buf: The display's 4-byte segment buffer.
  Painter(uint8_t *buf);

  // Turns on segments to draw a numeral or character
  // Digit: 0 is leftmost, 3 is rightmost.
  // Val: 0 thru 9, or a supported character.
  void setDigit(uint8_t digit, uint8_t val);

  // Turns on decimal digits/dots
  // Dots: DT_XX defines
  void setDots(uint8_t mask);

  // Turns on segments in a digit.
  // Digit: 0 is leftmost, 3 is rightmost.
  // Segment mask: SG_XX defines
  void setSegs(uint8_t digit, uint8_t mask);

  void setAllSegs(const uint8_t *pMasks);

  // Turns on a segment in a digit.
  // Digit: 0 is leftmost, 3 is rightmost.
  // Segment IX: 0 is top; 6 is bottom.
  void setSegIx(uint8_t digit, uint8_t segIx);

  // Turns off a segment in a digit.
  // Digit: 0 is leftmost, 3 is rightmost.
  // Segment IX: 0 is top; 6 is bottom.
  void clearSegIx(uint8_t digit, uint8_t segIx);

  // Queries a segment in a digit. Returns 1 if on, 0 if off.
  // Digit: 0 is leftmost, 3 is rightmost.
  // Segment IX: 0 is top; 6 is bottom.
  uint8_t getSegIx(uint8_t digit, uint8_t segIx);
};

#endif
