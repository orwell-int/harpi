#include "Switch.hpp"

#include <Arduino.h>

namespace harpi
{

Switch::Switch(uint8_t const pin)
  : m_pin(pin)
{
  pinMode(m_pin, INPUT);
}

Control Switch::read() const
{
  int const value = digitalRead(m_pin);
//  Serial.print("Switch read: ");
//  Serial.print(value);
//  Serial.print("\n");
  if (LOW == value)
  {
//    Serial.print("Switch -> LOW\n");
    return Control::Disabled;
  }
  else
  {
//    Serial.print("Switch -> HIGH\n");
    return Control::Enabled;
  }
}

}