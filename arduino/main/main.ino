#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include "Motor.hpp"
#include "Led.hpp"
#include "Switch.hpp"
#include "UltraSound.hpp"

// RFID needs 3.3V
uint8_t const RFID_RST_PIN = 10;
uint8_t const RFID_SDA_PIN = 9;

byte const KEY1[4] = {0xB7, 0x84 ,0x20, 0xD9};
byte const KEY2[4] = {0x60, 0x79, 0xFA, 0xA3};

// US needs 5V
uint8_t const US_TRIG_PIN = A0;
uint8_t const US_ECHO_PIN = A1;

uint8_t const MOTOR1_PWM_PIN = 3;
uint8_t const MOTOR1_IN1_PIN = 2;
uint8_t const MOTOR1_IN2_PIN = 4;

uint8_t const MOTOR2_PWM_PIN = 6;
uint8_t const MOTOR2_IN1_PIN = 5;
uint8_t const MOTOR2_IN2_PIN = 7;

// RGB LEDs need 5V
uint8_t const LED_PIN = A2;
uint8_t const LED_COUNT = 2;

// VOLTMETER
uint8_t const VOLTAGE_PIN = A3;
uint8_t const VOLTAGE_RATIO = 11;

// I2C
uint8_t const I2C_ADDRESS = 1;

harpi::Motor MOTOR1(MOTOR1_PWM_PIN, MOTOR1_IN1_PIN, MOTOR1_IN2_PIN);
harpi::Motor MOTOR2(MOTOR2_PWM_PIN, MOTOR2_IN1_PIN, MOTOR2_IN2_PIN);
harpi::UltraSound US(US_TRIG_PIN, US_ECHO_PIN);
MFRC522 RFID(RFID_SDA_PIN, RFID_RST_PIN);  // Create MFRC522 instance
Adafruit_NeoPixel LEDS = Adafruit_NeoPixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
harpi::Led LED1(0, LEDS);
harpi::Led LED2(1, LEDS);

void setup()
{
  Serial.begin(9600);
  SPI.begin(); // Init SPI bus
  RFID.PCD_Init(); // Init MFRC522
  LEDS.begin();

  Wire.begin(I2C_ADDRESS);
  Wire.onRequest(onI2cRequest);
  Wire.onReceive(onI2cReceive);
}

void loop()
{
  double distance = US.read();
  if (distance > 2000)
  {
    distance = 2000;
  }

  float voltage = analogRead(VOLTAGE_PIN) * VOLTAGE_RATIO * 5 / 1024.0;
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print(" mm ; voltage: ");
  Serial.print(voltage);
  Serial.print(" V");
  Serial.print("\n");

  MOTOR1.write();
  MOTOR2.write();

  if (LED1.write() or LED2.write()) {
    LEDS.show();  
  }

  delay(20);

  if (RFID.PICC_IsNewCardPresent() and RFID.PICC_ReadCardSerial())
  {
    Serial.print(F("The NUID tag is: "));
    printHex(RFID.uid.uidByte, RFID.uid.size);
    Serial.println();

    if (strncmp(RFID.uid.uidByte, KEY1, RFID.uid.size) == 0)
    {
      Serial.println("KEY1 detected!");
    }
    else if (strncmp(RFID.uid.uidByte, KEY2, RFID.uid.size) == 0)
    {
      Serial.println("KEY2 detected!");
    }
  }
  delay(400);
}

/**
 * Helper routine to dump a byte array as hex values to Serial.
 */
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
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

void onI2cReceive(int numBytes) {
  uint8_t const c = Wire.read();
  Serial.print(c);

  switch(static_cast<i2cCommand>(c)) {
    case i2cCommand::VALUE:
      // nothing to do
    break;
    case i2cCommand::LED1:
    {
      uint8_t const r = Wire.read();
      uint8_t const g = Wire.read();
      uint8_t const b = Wire.read();
      LED1.set(r, g, b);
      break;
    }
    case i2cCommand::LED2:
    {
      uint8_t const r = Wire.read();
      uint8_t const g = Wire.read();
      uint8_t const b = Wire.read();
      LED2.set(r, g, b);
      break;
    }
    case i2cCommand::MOTORS:
    {
      int8_t const dir1 = Wire.read();
      uint8_t const speed1 = Wire.read();
      MOTOR1.set(dir1, speed1);
      int8_t const dir2 = Wire.read();
      uint8_t const speed2 = Wire.read();
      MOTOR2.set(dir2, speed2);
      break;
    }
  }
  
  int x = Wire.read();
  Serial.println(x);
}

void onI2cRequest() {
  Wire.write("hello");
}
