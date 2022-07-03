#ifndef TEMP_H
#define TEMP_H

// Last measured temperature (C * 10).
// You're only supposed to read this. measureTemp() writes it.
extern volatile int16_t tempC;

// Last measured temperature (F)
extern volatile int16_t tempF;

void startTempConversion();
void readTemp();

#endif
