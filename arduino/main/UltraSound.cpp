#include "UltraSound.hpp"

#include <Arduino.h>
#include <algorithm>

namespace harpi
{

UltraSound::UltraSound(
  uint8_t const pinTrig,
  uint8_t const pinEcho,
  uint32_t const maxDistance)
  : m_pinTrig(pinTrig)
  , m_pinEcho(pinEcho)
  , m_maxDistance(maxDistance)
{
  pinMode(m_pinTrig, OUTPUT);
  pinMode(m_pinEcho, INPUT);
}

uint32_t UltraSound::read() const
{
  digitalWrite(m_pinTrig, LOW);
  delayMicroseconds(2);
  digitalWrite(m_pinTrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(m_pinTrig, LOW);
  long pulseWidth = pulseIn(m_pinEcho, HIGH);
  return std::min(m_maxDistance, long(pulseWidth / 5.8));
}

}
