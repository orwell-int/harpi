#pragma once

#include <esp32-hal-ledc.h>
#include <vector>
#include <stdexcept>

namespace harpi
{

template< typename T > struct Optional
{
public:
  Optional(T const & object)
  {
    m_object = new T;
    *m_object = object;
  }

  Optional()
    : m_object(nullptr)
  {
  }

  void operator=(Optional< T > const & other)
  {
    if (&other == this)
    {
      return;
    }
    *m_object = *other.m_object;
  }

  operator bool() const
  {
    return (nullptr != m_object);
  }

  T const & get() const
  {
    return *m_object;
  }

  T const & operator*() const
  {
    if (nullptr == m_object)
    {
      throw std::out_of_range("nullptr");
    }
    return *m_object;
  }
private:
  T * m_object;
};

struct Sound
{
  Sound(long const silenceDuration)
    : m_duration(silenceDuration)
    , m_octave(0)
  {
  }
  
  Sound(
    long const noteDuration,
    note_t const note,
    uint8_t const octave)
    : m_duration(noteDuration)
    , m_note(note)
    , m_octave(octave)
  {
  }
  
  Sound(
    int const noteDuration,
    note_t const note,
    uint8_t const octave,
    uint32_t const volume)
    : m_duration(noteDuration)
    , m_note(note)
    , m_octave(octave)
    , m_volume(volume)
  {
  }

//  void operator=(Sound const & other)
//  {
//    if (&other == this)
//    {
//      return;
//    }
//    m_duration = other.m_duration;
//    m_note = other.m_note;
//    m_octave = other.m_octave;
//    m_volume = other.m_volume;
//  }

  long m_duration;
  Optional< note_t > m_note;
  uint8_t m_octave;
  Optional< uint32_t > m_volume;
};

typedef std::vector< Sound > SoundVector;
}
