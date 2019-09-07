#include "Buzzer.hpp"

#include <algorithm>

namespace harpi
{

Buzzer::Buzzer(
  uint8_t const pin,
  int const channel)
  : m_pin(pin)
  , m_channel(channel)
  , m_currentSound(m_sounds.end())
{
  int const pwmFreq = 100;
  int const pwmResolution = 8;
  ledcSetup(channel, pwmFreq, pwmResolution);
  ledcAttachPin(pin, m_channel);
}

Buzzer::~Buzzer()
{
  silence();
}

void Buzzer::addSounds(SoundVector const & sounds)
{
  m_sounds.insert(m_sounds.end(), sounds.begin(), sounds.end());
}

void Buzzer::start(long time)
{
  m_startTime = time;
  if (m_sounds.empty())
  {
    return;
  }
  m_currentSound = m_sounds.begin();
  if (m_sounds.end() == m_currentSound)
  {
    return;
  }
  buzz();
}

void Buzzer::update(long time)
{
  if (m_sounds.end() == m_currentSound)
  {
    return;
  }
  Sound const & sound = *m_currentSound;
  if (m_startTime + sound.m_duration >= time)
  {
    ++m_currentSound;
  }
  buzz();
}

void Buzzer::buzz() const
{
  Sound const & sound = *m_currentSound;
  if (sound.m_note)
  {
    if (sound.m_volume)
    {
      ledcWrite(m_channel, *sound.m_volume);
    }
    ledcWriteNote(m_channel, *sound.m_note, sound.m_octave);
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
