#include "Buzzer.hpp"

#include <algorithm>
#include <HardwareSerial.h>

namespace harpi
{

Buzzer::Buzzer(
  uint8_t const pin,
  int const channel)
  : m_pin(pin)
  , m_channel(channel)
  , m_playing(false)
{
  int const pwmFreq = 100;
  int const pwmResolution = 8;
  ledcSetup(channel, pwmFreq, pwmResolution);
  ledcAttachPin(pin, m_channel);
}

Buzzer::~Buzzer()
{
  silence();
  m_playing = false;
}

void Buzzer::addSounds(SoundVector const & sounds)
{
  if (m_playing)
  {
    long lastStopTime =
      (m_sounds.empty())
      ? lastStopTime = m_stopTime
      : lastStopTime = m_sounds.back().getStopTime();
    for (auto const& sound: sounds)
    {
      TimedSound timedSound = { lastStopTime, sound };
      m_sounds.push_back(timedSound);
      lastStopTime = timedSound.getStopTime();
    }
  }
  else
  {
    for (auto const& sound: sounds)
    {
      m_sounds.push_back({ 0, sound });
    }
  }
}

void Buzzer::updateStartTimes(long const time)
{
  long startTime = time;
  for (auto& sound: m_sounds)
  {
    sound.m_startTime = startTime;
    startTime = sound.getStopTime();
  }
}

void Buzzer::start(long const time)
{
  if (m_sounds.empty())
  {
    return;
  }
  update(time);
}

void Buzzer::stop()
{
  silence();
  m_playing = false;
}

void Buzzer::update(long const time)
{
  Serial.println(time);
  if (not m_playing)
  {
    if (not m_sounds.empty())
    {
      TimedSound sound = m_sounds.front();
      m_sounds.pop_front();
      sound.m_startTime = time;
      m_stopTime = sound.getStopTime();
      buzz(sound.m_sound);
      updateStartTimes(m_stopTime);
    }
  }
  else
  {
    if (m_stopTime <= time)
    {
      bool foundSound = false;
      if (not m_sounds.empty())
      {
        while (not foundSound)
        {
          TimedSound sound = m_sounds.front();
          m_sounds.pop_front();
          if (sound.getStopTime() <= time)
          {
            sound.m_sound.print();
            if (m_sounds.empty())
            {
              break;
            }
            continue;
          }
          buzz(sound.m_sound);
          foundSound = true;
        }
      }
      if (not foundSound)
      {
        stop();
      }
    }
  }
}

void Buzzer::buzz(Sound const & sound)
{
  m_playing = true;
  if (sound.m_hasNote)
  {
    if (sound.m_hasVolume)
    {
      ledcWrite(m_channel, sound.m_volume);
    }
    ledcWriteNote(m_channel, sound.m_note, sound.m_octave);
  }
  else
  {
    silence();
  }
}

void Buzzer::sound(
  int const freq,
  int const volume) const
{
  ledcWriteTone(m_channel, freq);
  ledcWrite(m_channel, volume);
}

void Buzzer::silence() const
{
  ledcWriteTone(m_channel, 0);
}
  
}
