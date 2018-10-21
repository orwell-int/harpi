#ifndef HARPI_MOTOR
#define HARPI_MOTOR

#include <stdint.h>

namespace harpi
{

enum class Direction
{
  Forward,
  Reverse
};

class Motor
{
public:
  Motor(
    uint8_t const pinPWM,
    uint8_t const pinIN1,
    uint8_t const pinIN2);

  void set(
    int8_t const direction,
    uint8_t const valuePWM);

  bool write();
  
private:
  void updateDirection();
  void updateSpeed();
  uint8_t const m_pinPWM;
  uint8_t const m_pinIN1;
  uint8_t const m_pinIN2;
  Direction m_direction;
  Direction m_newDirection;
  uint8_t m_valuePWM;
  uint8_t m_newValuePWM;
  bool m_hasNewDirection;
  bool m_hasNewValuePWM;
};

}

#endif // #ifndef HARPI_MOTOR