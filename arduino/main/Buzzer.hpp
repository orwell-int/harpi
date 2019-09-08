#pragma once

#include "Sound.hpp"

#include <deque>

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

  void updateStartTimes(long const time);

  void start(long const time);

  void stop();

  void update(long const time);
  
  void sound(
    int const freq=200,
    int const volume=128) const;

  void silence() const;

private:
  void buzz(Sound const & sound);

  uint8_t const m_pin;
  int const m_channel;
  struct TimedSound
  {
    long m_startTime;
    Sound m_sound;

    long getStopTime() const
    {
      return m_startTime + m_sound.m_duration;
    }
  };
  typedef std::deque< TimedSound > TimedSoundDequeue;
  TimedSoundDequeue m_sounds;
  long m_stopTime;
  bool m_playing;
};
}
