#include "Buzzer.hpp"
#include "Sound.hpp"
#include <iostream>

int main()
{
  std::cout << "Test.Buzzer.cpp" << std::endl;
  harpi::Buzzer aBuzzer(1);
  long time(0);
  aBuzzer.start(time);
  harpi::SoundVector const sounds = {
    harpi::Sound(10, NOTE_C, 3), harpi::Sound(2),
    harpi::Sound(10, NOTE_D, 3), harpi::Sound(2)};
  aBuzzer.addSounds(sounds);
  uint16_t i = 1;
  for ( ; i <= 30 ; ++i)
  {
    std::clog << "At " << i << "\n";
    aBuzzer.update(++time);
  }
  aBuzzer.addSounds(sounds);
  for ( ; i <= 60 ; ++i)
  {
    std::clog << "At " << i << "\n";
    aBuzzer.update(++time);
  }
  return 0;
}
