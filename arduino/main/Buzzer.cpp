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
  , m_lastPlayedSound(m_sounds.end())
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
  bool const needNewPositionForCurrentSound(m_sounds.end() == m_currentSound);
  bool const needNewPositionForLastPlayedSound(m_sounds.end() == m_lastPlayedSound);
  SoundVector::const_iterator newEnd =  m_sounds.insert(m_sounds.end(), sounds.begin(), sounds.end());
  if (needNewPositionForCurrentSound)
  {
    m_currentSound = newEnd;
  }
  if (needNewPositionForLastPlayedSound)
  {
    m_lastPlayedSound = m_sounds.end();
  }
}

void Buzzer::start(long const time)
{
  if (m_sounds.empty())
  {
    return;
  }
  m_lastPlayedSound = m_sounds.end();
  m_currentSound = m_sounds.begin();
  buzz(time);
}

void Buzzer::update(long const time)
{
  if (m_sounds.end() == m_currentSound)
  {
    return;
  }
  if (m_playing)
  {
    Sound const & sound = *m_currentSound;
    if (m_startTime + sound.m_duration <= time)
    {
      ++m_currentSound;
    }
  }
  buzz(time);
}

void Buzzer::buzz(long const time)
{
  if (m_sounds.end() == m_currentSound)
  {
    return;
  }
  if (m_currentSound == m_lastPlayedSound)
  {
    return;
  }
  m_playing = true;
  m_startTime = time;
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
  m_lastPlayedSound = m_currentSound;
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
