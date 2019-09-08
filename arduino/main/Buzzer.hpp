#pragma once

#include "Sound.hpp"

namespace harpi
{
class Buzzer
{
public:
  Buzzer(
    uint8_t const pin,
    int const channel=0);

  ~Buzzer();

  void addSounds(SoundVector const & sounds);

  void start(long const time);

  void update(long const time);
  
  void sound(
    int const freq=200,
    int const volume=128) const;

  void silence() const;

private:
  void buzz(long const time);

  uint8_t const m_pin;
  int const m_channel;
  SoundVector m_sounds;
  SoundVector::const_iterator m_currentSound;
  SoundVector::const_iterator m_lastPlayedSound;
  long m_startTime;
  bool m_playing;
};
}
