#ifndef HARPI_LED
#define HARPI_LED

#include <stdint.h>

class Adafruit_NeoPixel;

namespace harpi
{

class Led
{
public:
  Led(
    uint8_t const index,
    Adafruit_NeoPixel & leds);

  void set(
    uint8_t const red,
    uint8_t const green,
    uint8_t const blue);

  bool write();

private:
  uint8_t const index;
  Adafruit_NeoPixel & leds;
  uint8_t red;
  uint8_t green;
  uint8_t blue;
  uint8_t newRed;
  uint8_t newGreen;
  uint8_t newBlue;
  bool hasNew;
};

}

#endif // #ifndef HARPI_LED