#ifndef TAG_FINDER_HPP
#define TAG_FINDER_HPP

#include <Arduino.h>
#include <MFRC522.h>

namespace harpi
{

class TagFinder
{
public:
  TagFinder(
    uint8_t const iPinSDA,
    uint8_t const iPinRST);

  void init();

  void read();

  bool hasDetected(MFRC522::Uid const & iTag);
private:
  MFRC522 m_reader;
  bool rfid_tag_present_prev;
  bool rfid_tag_present;
  int _rfid_error_counter;
  bool _tag_found;
};

} // namespace harpi

#endif // #ifndef TAG_FINDER_HPP