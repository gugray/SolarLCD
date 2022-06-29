#include <Arduino.h>

#define BUFLEN 4
uint8_t buf[BUFLEN];

void setup()
{
  Serial.begin(9600);
  delay(1000);
}

void loop()
{
  Serial.readBytesUntil('\n', buf, BUFLEN);
  int val = analogRead(A0);
  float voltage = (val * 5) / 1023.0;
  Serial.println(voltage);
}
