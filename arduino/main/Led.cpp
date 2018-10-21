#include "Led.hpp"

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

namespace harpi
{

Led::Led(
  uint8_t const index,
  Adafruit_NeoPixel & leds)
  : index(index)
  , leds(leds)
  , red(0)
  , green(0)
  , blue(0)
  , newRed(0)
  , newGreen(0)
  , newBlue(0)
  , hasNew(true)
{
}

void Led::set(
  uint8_t const redValue,
  uint8_t const greenValue,
  uint8_t const blueValue) {
  if (red != redValue || green != greenValue || blue != blueValue) {
    hasNew = true;
    newRed = redValue;
    newGreen = greenValue;
    newBlue = blueValue;
  }
}

bool Led::write()
{
  if (hasNew) {
    leds.setPixelColor(index, newRed, newGreen, newBlue);
    red = newRed;
    green = newGreen;
    blue = newBlue;
    hasNew = false;
    
    return true;
  }
  return false;
}

}