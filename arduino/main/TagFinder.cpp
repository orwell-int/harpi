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
//    and (iTag1.sak == iTag2.sak);
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
//      MFRC522::Uid const & tag = TAGS[i];
//      Serial.print("Plan to kill tag: ");
//      harpi::PrintHex(tag.uidByte, tag.size);
//      Serial.print(" at ");
//      Serial.print(i);
//      Serial.print(" age: ");
//      Serial.print(newAge);
//      Serial.print(" > ");
//      Serial.print(maxAge);
//      Serial.println("");
      TAGS_TO_KILL[TAGS_TO_KILL_COUNT++] = i;
    }
  }
  for (uint8_t j = 0 ; j < TAGS_TO_KILL_COUNT ; ++j)
  {
    uint8_t const i = TAGS_TO_KILL[j];
    --TAGS_COUNT;
//    Serial.print("Kill tag: ");
//    MFRC522::Uid const & tag = TAGS[i];
//    harpi::PrintHex(tag.uidByte, tag.size);
//    Serial.println("");
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
//  Serial.print("Add tag: ");
//  harpi::PrintHex(iTag.uidByte, iTag.size);
//  Serial.println("");
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
  //increase_age_and_kill_elders();
}

void clear_tags()
{
  TAGS_COUNT = 0;
}


TagFinder::TagFinder(
  uint8_t const iPinSDA,
  uint8_t const iPinRST)
  : m_reader(iPinSDA, iPinRST)
  , rfid_tag_present_prev(false)
  , rfid_tag_present(false)
  , _rfid_error_counter(0)
  , _tag_found(false)
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
  rfid_tag_present_prev = rfid_tag_present;

  _rfid_error_counter += 1;
  if(_rfid_error_counter > 2){
    _tag_found = false;
  }

  // Detect Tag without looking for collisions
  byte bufferATQA[2];
  byte bufferSize = sizeof(bufferATQA);

  MFRC522::StatusCode result = m_reader.PICC_RequestA(bufferATQA, &bufferSize);
  increase_age_and_kill_elders();
  if(result == MFRC522::STATUS_OK){
    if ( ! m_reader.PICC_ReadCardSerial()) { //Since a PICC placed get Serial and continue
//      Serial.println("! mfrc522.PICC_ReadCardSerial() -> return");
      return;
    }
//    Serial.print(F("The NUID tag is: "));
//    harpi::PrintHex(mfrc522.uid.uidByte, mfrc522.uid.size);
//    Serial.println();
    meta_add_tag(m_reader.uid);
    _rfid_error_counter = 0;
    _tag_found = true;
  }
  
  rfid_tag_present = _tag_found;
  
  // rising edge
  if (rfid_tag_present && !rfid_tag_present_prev){
//    Serial.println("Tag found");
  }
  
  // falling edge
  if (!rfid_tag_present && rfid_tag_present_prev){
//    Serial.println("Tag(s) gone");
    clear_tags();
  }
}

bool TagFinder::hasDetected(MFRC522::Uid const & iTag)
{
  uint8_t index;
  return is_tag_known(iTag, index);
}

} // namespace harpi