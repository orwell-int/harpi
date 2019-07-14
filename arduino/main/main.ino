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

#include <WiFi.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>

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

WiFiUDP UDP;
IPAddress PROXY_ADDRESS;
int PROXY_PORT = 0;

String const REBOOT_SERIAL_COMMAND("reboot\n");


int as_int(char * const chars, uint8_t len=0)
{
  if (0 == len)
  {
    len = strlen(chars);
  }
  String string;
  for (uint8_t i = 0 ; i < len ; ++i)
  {
    string += char(chars[i]);
  }
  try
  {
    return string.toInt();
  }
  catch (...)
  {
    Serial.print("This was not an int: ");
    Serial.println(chars);
  }
  return 0;
}

void setup()
{
  Serial.begin(9600);
  SPI.begin(); // Init SPI bus
  TAG_FINDER.init(); // Init MFRC522
  LEDS.begin();

  BUZZER.sound(1000, 64);
  delay(400);
  BUZZER.silence();

  WiFi.begin();
  uint8_t tries = 0;
  uint8_t const MAX_TRIES = 10;
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    if (++tries >= MAX_TRIES)
    {
      Serial.println("WiFi connection failed, begin again");
      BUZZER.sound(800, 64);
      delay(200);
      BUZZER.sound(700, 64);
      delay(200);
      BUZZER.sound(600, 64);
      delay(200);
      BUZZER.silence();
      tries = 0;
      WiFi.begin();
    }
    delay(1000);
  }
  Serial.println("WiFi connected");
  IPAddress address = WiFi.localIP();
  Serial.print("IP is ");
  Serial.println(address);

  BUZZER.sound(900, 64);
  delay(100);
  BUZZER.sound(1200, 64);
  delay(300);
  BUZZER.silence();

  UDP.begin(9080);

  IPAddress broadcast_address = address;
  broadcast_address[3] = 255; //hack
  bool received = false;
  while (not received)
  {
    uint8_t buffer[50] = "esp32";
    UDP.beginPacket(broadcast_address, 9081);
    UDP.write(buffer, strlen((char *)buffer));
    UDP.endPacket();
    delay(100);
    memset(buffer, 0, 50);
    for (uint8_t i = 0 ; i < 5 ; ++i)
    {
      //processing incoming packet, must be called before reading the buffer
      int len = UDP.parsePacket();
      if (len > 0)
      {
        UDP.read(buffer, 50);
        PROXY_ADDRESS = UDP.remoteIP();
        Serial.print("Reply to broadcast from proxy : ");
        Serial.println((char *)buffer);
        Serial.print("Proxy address: ");
        Serial.println(PROXY_ADDRESS);
        received = true;
        PROXY_PORT = as_int((char *)buffer, len);
        Serial.print("Proxy port: ");
        Serial.println(PROXY_PORT);
        BUZZER.sound(800, 64);
        delay(400);
        BUZZER.silence();
        // echo to proxy
        UDP.beginPacket(PROXY_ADDRESS, PROXY_PORT);
        UDP.write(buffer, len);
        UDP.endPacket();
        break;
      }
      else
      {
        Serial.println("No reply yet");
        delay(1000);
      }
    }
    if (not received)
    {
      Serial.println("No reply to UDP broadcast");
      delay(1000);
    }
  }

  Serial.println("Proxy OK\n");
  BUZZER.sound(700, 64);
  delay(200);
  BUZZER.sound(900, 64);
  delay(200);
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
  if (Serial.available() > 0)
  {
    String str(Serial.readString());
    Serial.print("Serial: '");
    Serial.print(str);
    Serial.println("'");
    if (REBOOT_SERIAL_COMMAND == str)
    {
      ESP.restart();
    }
  }
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

  uint8_t const MAX_SIZE = 100;
  uint8_t buffer[MAX_SIZE];
  uint8_t move_left = 0;
  uint8_t move_right = 0;
  bool can_move = false;
  memset(buffer, 0, MAX_SIZE);
  int len = UDP.parsePacket();
  if (len > 0)
  {
    UDP.read(buffer, MAX_SIZE);
    // "move {left} {right}"
    Serial.print("Message of length ");
    Serial.print(len);
    Serial.print(" received: ");
    Serial.println((char *)buffer);
    char * msg = strtok((char *)buffer, " ");
    if (nullptr != msg)
    {
      if (0 == strcmp("move", msg))
      {
        char * left = strtok(nullptr, " ");
        if (nullptr != msg)
        {
          move_left = as_int(left);
          char * right = strtok(nullptr, " ");
          if (nullptr != msg)
          {
            move_right = as_int(right);
            can_move = true;
          }
          else
          {
            Serial.println("Could not read right");
          }
        }
        else
        {
          Serial.println("Could not read left");
        }
      }
      else
      {
        Serial.println("Wrong type of message");
      }
    }
    else
    {
      Serial.println("Could not read message");
    }
  }

  harpi::Direction direction_left = harpi::Direction::Forward;
  harpi::Direction direction_right = harpi::Direction::Forward;
  if (can_move)
  {
    char dleft = '+';
    char dright = '+';
    if (move_left < 0)
    {
      direction_left = harpi::Direction::Reverse;
      move_left = -move_left;
      dleft = '-';
    }
    if (move_right < 0)
    {
      direction_right = harpi::Direction::Reverse;
      move_right = -move_right;
      dright = '-';
    }
    Serial.print("Move: ");
    Serial.print(dleft);
    Serial.print(" ");
    Serial.print(move_left);
    Serial.print("/");
    Serial.print(dright);
    Serial.print(" ");
    Serial.println(move_right);
  }
  else
  {
    move_left = 0;
    move_right = 0;
  }

  MOTOR1.set(direction_left, move_left);  // 170 is too low ; movement at 190
  MOTOR2.set(direction_right, move_right);

  MOTOR1.write();
  MOTOR2.write();

  LED1.set(static_cast< uint8_t >(map(distance, 0, 2000, 0, 255)), 20, 20);

  sprintf((char *)buffer, "distance %i", distance);
  UDP.beginPacket(PROXY_ADDRESS, PROXY_PORT);
  UDP.write(buffer, strlen((char *)buffer));
  UDP.endPacket();

  if (LED1.write() or LED2.write())
  {
    LEDS.show();  
  }

  delay(400);
}
