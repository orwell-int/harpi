#include "TagFinder.hpp"


#include "Utils.hpp"

namespace harpi
{

uint8_t const MAX_TAGS = 5;
MFRC522::Uid TAGS[MAX_TAGS];
uint8_t TAGS_COUNT = 0;

uint8_t TAGS_AGE[MAX_TAGS];

bool are_tags_equal(
  MFRC522::Uid const & iTag1,
  MFRC522::Uid const & iTag2)
{
  return (iTag1.size == iTag2.size)
    and (strncmp(iTag1.uidByte, iTag2.uidByte, iTag1.size) == 0);
}

bool is_tag_known(
  MFRC522::Uid const & iTag,
  uint8_t & oIndex)
{
  for (uint8_t i = 0 ; i < TAGS_COUNT ; ++i)
  {
    MFRC522::Uid const & tag = TAGS[i];
    if (are_tags_equal(iTag, tag))
    {
      oIndex = i;
      return true;
    }
  }
  return false;
}

uint8_t TAGS_TO_KILL[MAX_TAGS];
uint8_t TAGS_TO_KILL_COUNT = 0;

void increase_age_and_kill_elders()
{
  TAGS_TO_KILL_COUNT = 0;
  uint8_t const maxAge = TAGS_COUNT + 2;
  for (uint8_t i = 0 ; i < TAGS_COUNT ; ++i)
  {
    uint8_t const newAge = ++TAGS_AGE[i];
    if (newAge > maxAge)
    {
      // we need to kill this tag
      TAGS_TO_KILL[TAGS_TO_KILL_COUNT++] = i;
    }
  }
  for (uint8_t j = 0 ; j < TAGS_TO_KILL_COUNT ; ++j)
  {
    uint8_t const i = TAGS_TO_KILL[j];
    --TAGS_COUNT;
    for (uint8_t k = i ; k < TAGS_COUNT ; ++k)
    {
      TAGS[k] = TAGS[k + 1];
      TAGS_AGE[k] = TAGS_AGE[k + 1];
    }
  }
}

void add_tag(MFRC522::Uid const & iTag)
{
  if (MAX_TAGS == TAGS_COUNT)
  {
    Serial.println("Max number of tags reached. Ignore new one.");
    return;
  }
  TAGS[TAGS_COUNT] = iTag;
  TAGS_AGE[TAGS_COUNT] = 0;
  ++TAGS_COUNT;
}

void meta_add_tag(MFRC522::Uid const & iTag)
{
  uint8_t index;
  if (is_tag_known(iTag, index))
  {
    TAGS_AGE[index] = 0;
  }
  else
  {
    add_tag(iTag);
  }
}

void clear_tags()
{
  TAGS_COUNT = 0;
}


TagFinder::TagFinder(
  uint8_t const iPinSDA,
  uint8_t const iPinRST)
  : m_reader(iPinSDA, iPinRST)
  , m_rfid_tag_present_prev(false)
  , m_rfid_tag_present(false)
  , m_rfid_error_counter(0)
  , m_tag_found(false)
{
}

void TagFinder::init()
{
  m_reader.PCD_Init();   // Init MFRC522
  
  // Reset baud rates
  m_reader.PCD_WriteRegister(m_reader.TxModeReg, 0x00);
  m_reader.PCD_WriteRegister(m_reader.RxModeReg, 0x00);
  // Reset ModWidthReg
  m_reader.PCD_WriteRegister(m_reader.ModWidthReg, 0x26);
}

void TagFinder::read()
{
  m_rfid_tag_present_prev = m_rfid_tag_present;

  m_rfid_error_counter += 1;
  if (m_rfid_error_counter > 2) {
    m_tag_found = false;
  }

  // Detect Tag without looking for collisions
  byte bufferATQA[2];
  byte bufferSize = sizeof(bufferATQA);

  MFRC522::StatusCode result = m_reader.PICC_RequestA(bufferATQA, &bufferSize);
  increase_age_and_kill_elders();
  if (result == MFRC522::STATUS_OK) {
    if (not m_reader.PICC_ReadCardSerial()) {
      // this might be an error
      return;
    }
    meta_add_tag(m_reader.uid);
    m_rfid_error_counter = 0;
    m_tag_found = true;
  }

  m_rfid_tag_present = m_tag_found;

  // falling edge
  if (not m_rfid_tag_present && m_rfid_tag_present_prev) {
    clear_tags();
  }
}

bool TagFinder::hasDetected(MFRC522::Uid const & iTag)
{
  uint8_t index;
  return is_tag_known(iTag, index);
}

} // namespace harpi