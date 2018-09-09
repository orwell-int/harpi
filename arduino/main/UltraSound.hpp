#ifndef HARPI_ULTRA_SOUND
#define HARPI_ULTRA_SOUND

#include <stdint.h>

class Adafruit_NeoPixel;

namespace harpi
{

class UltraSound
{
public:
  UltraSound(
    uint8_t const pinTrig,
    uint8_t const pinEcho);

  double read() const;

private:
  uint8_t const m_pinTrig;
  uint8_t const m_pinEcho;
};

}

#endif // #ifndef HARPI_ULTRA_SOUND