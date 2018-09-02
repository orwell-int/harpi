#ifndef HARPI_SWITCH
#define HARPI_SWITCH

#include <stdint.h>

namespace harpi
{

enum class Control
{
  Disabled,
  Enabled
};

class Switch
{
public:
  Switch(uint8_t const pin);

  Control read() const;

private:
  uint8_t const m_pin;
};

}

#endif // #ifndef HARPI_SWITCH