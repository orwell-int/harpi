#include "UltraSound.hpp"

#include <Arduino.h>

namespace harpi
{

UltraSound::UltraSound(
  uint8_t const pinTrig,
  uint8_t const pinEcho)
  : m_pinTrig(pinTrig)
  , m_pinEcho(pinEcho)
{
  pinMode(m_pinTrig, OUTPUT);
  pinMode(m_pinEcho, INPUT);
}

double UltraSound::read() const
{
  digitalWrite(m_pinTrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(m_pinTrig, LOW);
  long pulseWidth = pulseIn(m_pinEcho, HIGH);
  return (pulseWidth / 5.8);
}

}