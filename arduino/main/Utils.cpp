#include "Utils.hpp"

namespace harpi
{

void PrintHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0x0" : " 0x");
    Serial.print(buffer[i], HEX);
  }
}

} // namespace harpi
