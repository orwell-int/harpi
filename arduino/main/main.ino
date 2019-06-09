#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include "Motor.hpp"
#include "Led.hpp"
#include "Switch.hpp"
#include "UltraSound.hpp"
#include "TagFinder.hpp"
#include "Utils.hpp"
#include "FakeTagFinder.hpp"
#include "Buzzer.hpp"

// /!\ GPIO 12 MUST NOT BE USED TO BE ABLE TO UPLOAD PROGRAM
// /!\ AND TO CONNECT TO SERIAL

// RFID needs 3.3V
// 18 SCK
// 19 MISO
// 23 MOSI
uint8_t const RFID_RST_PIN = 22;
uint8_t const RFID_SDA_PIN = 21;

MFRC522::Uid KEY1 = {4, {0xB7, 0x84, 0x20, 0xD9}, 0};
MFRC522::Uid KEY2 = {4, {0x60, 0x79, 0xFA, 0xA3}, 0};
// other key with ring: B6 35 EA F7

bool KEY1_PRESENT = false;
bool KEY2_PRESENT = false;

bool OLD_KEY1_PRESENT = false;
bool OLD_KEY2_PRESENT = false;

// US needs 5V
uint8_t const US_TRIG_PIN = 4;  // 34 not working
uint8_t const US_ECHO_PIN = 5;  // 35 not working

uint8_t const MOTOR1_PWM_PIN = 14; // EN
uint8_t const MOTOR1_IN1_PIN = 26;
uint8_t const MOTOR1_IN2_PIN = 27;

uint8_t const MOTOR2_PWM_PIN = 25; // EN
uint8_t const MOTOR2_IN1_PIN = 32;
uint8_t const MOTOR2_IN2_PIN = 33;

// RGB LEDs need 5V
uint8_t const LED_PIN = 15;
uint8_t const LED_COUNT = 2;

// VOLTMETER
uint8_t const VOLTAGE_PIN = A6; // GPIO 34
uint8_t const VOLTAGE_RATIO = 11;

// BUZZER
uint8_t const BUZZ_PIN = 13;

double const ADC_MAX_VALUE = 4095.0;

harpi::Motor MOTOR1(MOTOR1_PWM_PIN, MOTOR1_IN1_PIN, MOTOR1_IN2_PIN);
harpi::Motor MOTOR2(MOTOR2_PWM_PIN, MOTOR2_IN1_PIN, MOTOR2_IN2_PIN, harpi::LogicToMotion::Pin1LowIsForward);
harpi::UltraSound US(US_TRIG_PIN, US_ECHO_PIN);
harpi::TagFinder TAG_FINDER(RFID_SDA_PIN, RFID_RST_PIN);
Adafruit_NeoPixel LEDS = Adafruit_NeoPixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
harpi::Led LED1(0, LEDS);
harpi::Led LED2(1, LEDS);
harpi::Buzzer BUZZER(BUZZ_PIN);

void setup()
{
  Serial.begin(9600);
  SPI.begin(); // Init SPI bus
  TAG_FINDER.init(); // Init MFRC522
  LEDS.begin();

  BUZZER.sound(1000, 64);
  delay(400);
  BUZZER.silence();


//  bip(1000, 64, 120);
//  note_t const notes[] = { NOTE_C, NOTE_D, NOTE_E, NOTE_F, NOTE_G, NOTE_A, NOTE_B };
//  for (auto const note: notes)
//  {
//    ledcWriteNote(channel, note, 4);
//    delay(400);
//  }
//  ledcWriteNote(channel, NOTE_C, 5);
//  delay(400);
//  silence();

}

double DISTANCE = 0;
double VOLTAGE = 0;
uint8_t RFID_CARDS_NUMBERS = 0;


void loop()
{
  TAG_FINDER.read();
  if (TAG_FINDER.hasDetected(KEY1))
  {
    KEY1_PRESENT = true;
    LED2.set(255, 255, 0);
    if (not OLD_KEY1_PRESENT)
    {
      Serial.println("Key1 appeared");
      LED2.set(255, 0, 0);
      //bip();
    }
  }
  else
  {
    KEY1_PRESENT = false;
    if (OLD_KEY1_PRESENT)
    {
      LED2.set(0, 0, 0);
      Serial.println("Key1 disappeared");
    }
  }
  if (TAG_FINDER.hasDetected(KEY2))
  {
    KEY2_PRESENT = true;
    LED2.set(0, 255, 255);
    if (not OLD_KEY2_PRESENT)
    {
      LED2.set(0, 0, 255);
      Serial.println("Key2 appeared");
      //bip();
    }
  }
  else
  {
    KEY2_PRESENT = false;
    if (OLD_KEY2_PRESENT)
    {
      LED2.set(0, 0, 0);
      Serial.println("Key2 disappeared");
    }
  }
  OLD_KEY1_PRESENT = KEY1_PRESENT;
  OLD_KEY2_PRESENT = KEY2_PRESENT;

  uint32_t distance = US.read();

  VOLTAGE = analogRead(VOLTAGE_PIN) * VOLTAGE_RATIO * 5 / ADC_MAX_VALUE;
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print(" mm ; voltage: ");
  Serial.print(VOLTAGE);
  Serial.print(" V");
  Serial.print("\n");

  MOTOR1.set(harpi::Direction::Forward, 0);  // 170 is too low ; movement at 190
  MOTOR2.set(harpi::Direction::Forward, 0);

  MOTOR1.write();
  MOTOR2.write();

  LED1.set(static_cast< uint8_t >(map(distance, 0, 2000, 0, 255)), 20, 20);

  if (LED1.write() or LED2.write()) {
    LEDS.show();  
  }

  delay(400);
}
