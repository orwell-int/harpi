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
    uint8_t const pinEcho,
    uint32_t const maxDistance=2000);

  uint32_t read() const;

private:
  uint8_t const m_pinTrig;
  uint8_t const m_pinEcho;
  long const m_maxDistance;
};

}

#endif // #ifndef HARPI_ULTRA_SOUND
