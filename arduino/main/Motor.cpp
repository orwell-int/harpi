#include "Motor.hpp"

#include <Arduino.h>

#include <analogWrite.h>

namespace harpi
{

Motor::Motor(
  uint8_t const pinPWM,
  uint8_t const pinIN1,
  uint8_t const pinIN2)
  : m_pinPWM(pinPWM)
  , m_pinIN1(pinIN1)
  , m_pinIN2(pinIN2)
  , m_direction(Direction::Forward)
  , m_newDirection(Direction::Forward)
  , m_valuePWM(0)
  , m_newValuePWM(0)
  , m_hasNewDirection(true)
  , m_hasNewValuePWM(true)
{
  pinMode(m_pinPWM, OUTPUT);
  pinMode(m_pinIN1, OUTPUT);
  pinMode(m_pinIN2, OUTPUT);
}

void Motor::set(
  int8_t const directionRaw,
  uint8_t const valuePWM)
{
  Direction const direction =
    (directionRaw >= 0)
    ? Direction::Forward
    : Direction::Reverse;
  if (direction != m_direction)
  {
    m_newDirection = direction;
    m_hasNewDirection = true;
  }
  if (valuePWM != m_valuePWM)
  {
    m_newValuePWM = valuePWM;
    m_hasNewValuePWM = true;
  }
}

bool Motor::write()
{
  bool hasChanged(false);
  if (m_hasNewDirection)
  {
    switch (m_newDirection)
    {
      case Direction::Forward:
      {
        digitalWrite(m_pinIN1, HIGH);
        digitalWrite(m_pinIN2, LOW);
        break;
      }
      case Direction::Reverse:
      {
        digitalWrite(m_pinIN1, LOW);
        digitalWrite(m_pinIN2, HIGH);
        break;
      }
    }
    m_direction = m_newDirection;
    m_hasNewDirection = false;
    hasChanged = true;
  }
  if (m_hasNewValuePWM)
  {
    analogWrite(m_pinPWM, m_newValuePWM);
    m_valuePWM = m_newValuePWM;
    m_hasNewValuePWM = false;
    hasChanged = true;
  }
}

}
