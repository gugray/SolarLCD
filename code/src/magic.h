#ifndef MAGIC_H
#define MAGIC_H



#define BLINK_THRESHOLD_VCC   300
#define LCD_THRESHOLD_VCC     240


#ifdef ENV_ATTINY
#define PIN_CS    4
#define PIN_WR    2
#define PIN_DATA  1
#define PIN_LED   0
#endif

#ifdef ENV_NANO
#define PIN_CS    11
#define PIN_WR    12
#define PIN_DATA  7
#define PIN_LED   13
#endif

#endif
