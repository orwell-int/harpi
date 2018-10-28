#ifndef HARPI_UTILS_HPP
#define HARPI_UTILS_HPP

#include <Arduino.h>

namespace harpi
{

/**
 * Helper routine to dump a byte array as hex values to Serial.
 */
void PrintHex(byte *buffer, byte bufferSize);

} // namespace harpi

#endif // #ifndef HARPI_UTILS_HPP
