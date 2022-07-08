#include <Arduino.h>
#include "euclidean.h"

const uint8_t rhythm1[1] = {0};
const uint8_t rhythm2[2] = {0, 8};
const uint8_t rhythm3[3] = {0, 5, 10};
const uint8_t rhythm4[4] = {0, 4, 8, 12};
const uint8_t rhythm5[5] = {0, 3, 6, 9, 12};
const uint8_t rhythm6[6] = {0, 3, 5, 8, 11, 13};
const uint8_t rhythm7[7] = {0, 3, 5, 7, 10, 12, 14};
const uint8_t rhythm8[8] = {0, 2, 4, 6, 8, 10, 12, 14};

const uint8_t *rhythms[8] = {
  rhythm1, rhythm2, rhythm3, rhythm4,
  rhythm5, rhythm6, rhythm7, rhythm8 };

bool isInArr(const uint8_t *arr, uint8_t len, uint8_t val)
{
  for (uint8_t i = 0; i < len; ++i)
  {
    if (arr[i] == val)
      return true;
  }
  return false;
}

bool isOnBeat(uint8_t nBeats, uint8_t offset, uint8_t currBeat)
{
  uint8_t countWithOffset = (currBeat + offset) % 16;
  if (nBeats == 0 || nBeats > 8)
    return false;
  return isInArr(rhythms[nBeats - 1], nBeats, countWithOffset);
  // if (nBeats == 1)
  //   return isInArr(rhythm1, 1, countWithOffset);
  // else if (nBeats == 2)
  //   return isInArr(rhythm2, 2, countWithOffset);
  // else if (nBeats == 3)
  //   return isInArr(rhythm3, 3, countWithOffset);
  // else if (nBeats == 4)
  //   return isInArr(rhythm4, 4, countWithOffset);
  // else if (nBeats == 5)
  //   return isInArr(rhythm5, 5, countWithOffset);
  // else if (nBeats == 6)
  //   return isInArr(rhythm6, 6, countWithOffset);
  // else if (nBeats == 7)
  //   return isInArr(rhythm7, 7, countWithOffset);
  // else if (nBeats == 8)
  //   return isInArr(rhythm8, 8, countWithOffset);
  // else
  //   return false;
}
