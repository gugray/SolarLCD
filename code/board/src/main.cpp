#include <Arduino.h>
#include "globals.h"
#include "loops.h"
#include "magic.h"

#include "sleep.h"

void setup()
{
  pinMode(PIN_LED, INPUT);
  ht1621.begin();
  currentLoopFun = startupLoop;

  // 2:00 -> 2: 18

  uint16_t counter = 0;
  while (true)
  {
    sleep(1); // 32 msec
    
    if ((counter++ % 16) != 0)
      continue;
    
    ht1621.clearBuffer();

    uint32_t totalSec = msec / 1000;
    uint8_t sec = totalSec % 60;
    uint32_t totalMin = totalSec / 60;
    uint8_t min = totalMin % 100;

    painter.setDigit(3, sec % 10);
    painter.setDigit(2, sec / 10);
    painter.setDigit(1, min % 10);
    painter.setDigit(0, min / 10);
    painter.setDots(DT_MDEC | DT_MDOT);
    ht1621.wrBuffer();
  }

  // cli();
  // // Clear timer counter
  // TCNT1 = 0;
  // // Enable interrupt for timer overflow
  // TIMSK |= (1 << OCIE1A);
  // // Interrupt when count is at N
  // OCR1A = 250;
  // // Prescaler
  // // CK:  (1 << CS12) | (1 << CS10)
  // // PCK: (1 << CS10)
  // TCCR1 = (1 << CS10);
  // sei();

}

// ISR(TIMER1_COMPA_vect)
// {
//   ++tcnt;
//   TCNT1 = 0;
// }

void loop()
{
  currentLoopFun();
}
