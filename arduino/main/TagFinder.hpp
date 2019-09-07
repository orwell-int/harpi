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
  bool m_rfid_tag_present_prev;
  bool m_rfid_tag_present;
  int m_rfid_error_counter;
  bool m_tag_found;
};

} // namespace harpi

#endif // #ifndef TAG_FINDER_HPP