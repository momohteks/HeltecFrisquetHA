#ifndef UTILS_H
#define UTILS_H

#include <heltec.h>

String byteArrayToHexString(uint8_t *byteArray, int length) {
  String result = "";
  for (int i = 0; i < length; i++)
  {
    char hex[3];
    sprintf(hex, "%02X", byteArray[i]);
    result += hex;
  }
  return result;
}

#endif