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

// RFID needs 3.3V
// 18 SCK
// 19 MISO
// 23 MOSI
uint8_t const RFID_RST_PIN = 22;
uint8_t const RFID_SDA_PIN = 21;

MFRC522::Uid KEY1 = {4, {0xB7, 0x84 ,0x20, 0xD9}, 0};
MFRC522::Uid KEY2 = {4, {0x60, 0x79, 0xFA, 0xA3}, 0};

bool KEY1_PRESENT = false;
bool KEY2_PRESENT = false;

bool OLD_KEY1_PRESENT = false;
bool OLD_KEY2_PRESENT = false;

// US needs 5V
uint8_t const US_TRIG_PIN = 34;
uint8_t const US_ECHO_PIN = 35;

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
uint8_t const VOLTAGE_PIN = 13;
uint8_t const VOLTAGE_RATIO = 11;

// BUZZER
uint8_t const BUZZ_PIN = 12;

double const ADC_MAX_VALUE = 4095.0;

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

harpi::Motor MOTOR1(MOTOR1_PWM_PIN, MOTOR1_IN1_PIN, MOTOR1_IN2_PIN);
harpi::Motor MOTOR2(MOTOR2_PWM_PIN, MOTOR2_IN1_PIN, MOTOR2_IN2_PIN);
harpi::UltraSound US(US_TRIG_PIN, US_ECHO_PIN);
harpi::TagFinder TAG_FINDER(RFID_SDA_PIN, RFID_RST_PIN);
Adafruit_NeoPixel LEDS = Adafruit_NeoPixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
harpi::Led LED1(0, LEDS);
harpi::Led LED2(1, LEDS);

int channel = 0;
  
void setup()
{
  Serial.begin(9600);
  SPI.begin(); // Init SPI bus
  TAG_FINDER.init(); // Init MFRC522
  LEDS.begin();

  int freq = 2000;
  int resolution = 8;
  ledcSetup(channel, freq, resolution);
  ledcAttachPin(BUZZ_PIN, channel);
  ledcWriteTone(channel, 2000);
}

double DISTANCE = 0;
double VOLTAGE = 0;
uint8_t RFID_CARDS_NUMBERS = 0;

void bip()
{
  ledcWrite(channel, 20);
  delay(70);
  ledcWrite(channel, 0);
}

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
      bip();
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
      bip();
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

  DISTANCE = US.read();

  VOLTAGE = analogRead(VOLTAGE_PIN) * VOLTAGE_RATIO * 5 / ADC_MAX_VALUE;
  Serial.print("Distance: ");
  Serial.print(DISTANCE);
  Serial.print(" mm ; voltage: ");
  Serial.print(VOLTAGE);
  Serial.print(" V");
  Serial.print("\n");

  MOTOR1.set(1, 255);
  MOTOR2.set(1, 255);

  MOTOR1.write();
  MOTOR2.write();

  if (LED1.write() or LED2.write()) {
    LEDS.show();  
  }

  delay(400);
}

enum class i2cCommand {
  VALUE,
  LED1,
  LED2,
  MOTORS
};

enum class i2cValue {
  US,
  RFID,
  VOLTAGE,
};
