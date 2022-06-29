#ifndef PAINTER_H
#define PAINTER_H

#define SG_TP   0x80  // Top horizontal
#define SG_TL   0x40  // Top left
#define SG_TR   0x20  // Top right
#define SG_MD   0x10  // Middle horizontal
#define SG_BL   0x08  // Bottom left
#define SG_BR   0x04  // Bottom right
#define SG_BM   0x02  // Bottom horizontal

#define DT_LDEC 0x80  // Left decimal
#define DT_MDEC 0x40  // Mid decimal
#define DT_MDOT 0x20  // Mid dot
#define DT_DEGR 0x10  // Degree sign

class Painter
{
public:
  enum Images
  {
    frog,
    helo,
  };

private:
  uint8_t *buf;

public:
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

  void setImage(Images img);
};

#endif
