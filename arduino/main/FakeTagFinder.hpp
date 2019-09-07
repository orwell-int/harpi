#ifndef FAKE_TAG_FINDER_HPP
#define FAKE_TAG_FINDER_HPP

#include <stdint.h>
#include <MFRC522.h>

namespace harpi
{
class FakeTagFinder
{
public:
  FakeTagFinder(
    uint8_t const iPinSDA,
    uint8_t const iPinRST)
  {
  }

  void init()
  {
  }

  void read()
  {
  }

  bool hasDetected(MFRC522::Uid const & iTag)
  {
    return false;
  };
};
}

#endif
