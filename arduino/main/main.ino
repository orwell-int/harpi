#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>

// RFID needs 3.3V
uint8_t const RFID_RST_PIN = 10;
uint8_t const RFID_SDA_PIN = 9;

byte const KEY1[4] = {0xB7,0x84,0x20,0xD9};
byte const KEY2[4] = {0x60,0x79,0xFA,0xA3};

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
uint8_t const MAX_LIGHT = 64;

// VOLTMETER
uint8_t const VOLTAGE_PIN = A3;
uint8_t const VOLTAGE_RATIO = 11;

// MOTORS
uint8_t const MIN_PWM = 50;
uint8_t const MAX_PWM = 255;
bool isClockwise = false;
bool IncreaseSpeed = true;

// I2C
uint8_t const I2C_ADDRESS = 1;

enum class Direction
{
  Forward,
  Reverse
};

enum class Control
{
  Disabled,
  Enabled  
};

class Motor
{
public:
  Motor(
    uint8_t const pinPWM,
    uint8_t const pinIN1,
    uint8_t const pinIN2);

  void set(
    int8_t const direction,
    uint8_t const valuePWM);

  bool write();
  
private:
  void updateDirection();
  void updateSpeed();
  uint8_t const m_pinPWM;
  uint8_t const m_pinIN1;
  uint8_t const m_pinIN2;
  Direction m_direction;
  Direction m_newDirection;
  uint8_t m_valuePWM;
  uint8_t m_newValuePWM;
  bool m_hasNewDirection;
  bool m_hasNewValuePWM;
};

Motor::Motor(
  uint8_t const pinPWM,
  uint8_t const pinIN1,
  uint8_t const pinIN2)
  : m_pinPWM(pinPWM)
  , m_pinIN1(pinIN1)
  , m_pinIN2(pinIN2)
  , m_direction(Direction::Forward)
  , m_newDirection(Direction::Forward)
  , m_valuePWM(0)
  , m_newValuePWM(0)
  , m_hasNewDirection(true)
  , m_hasNewValuePWM(true)
{
  pinMode(m_pinPWM, OUTPUT);
  pinMode(m_pinIN1, OUTPUT);
  pinMode(m_pinIN2, OUTPUT);
}

void Motor::set(
  int8_t const directionRaw,
  uint8_t const valuePWM)
{
  Direction const direction =
    (directionRaw >= 0)
    ? Direction::Forward
    : Direction::Reverse;
  if (direction != m_direction)
  {
    m_newDirection = direction;
    m_hasNewDirection = true;
  }
  if (valuePWM != m_valuePWM)
  {
    m_newValuePWM = valuePWM;
    m_hasNewValuePWM = true;
  }
}

bool Motor::write()
{
  bool hasChanged(false);
  if (m_hasNewDirection)
  {
    switch (m_newDirection)
    {
      case Direction::Forward:
      {
        digitalWrite(m_pinIN1, HIGH);
        digitalWrite(m_pinIN2, LOW);
        break;
      }
      case Direction::Reverse:
      {
        digitalWrite(m_pinIN1, LOW);
        digitalWrite(m_pinIN2, HIGH);
        break;
      }
    }
    m_direction = m_newDirection;
    m_hasNewDirection = false;
    hasChanged = true;
  }
  if (m_hasNewValuePWM)
  {
    analogWrite(m_pinPWM, m_newValuePWM);
    m_valuePWM = m_newValuePWM;
    m_hasNewValuePWM = false;
    hasChanged = true;
  }
}

class Switch
{
public:
  Switch(uint8_t const pin);

  Control read() const;

private:
  uint8_t const m_pin;
};

Switch::Switch(uint8_t const pin)
  : m_pin(pin)
{
  pinMode(m_pin, INPUT);
}

Control Switch::read() const
{
  int const value = digitalRead(m_pin);
//  Serial.print("Switch read: ");
//  Serial.print(value);
//  Serial.print("\n");
  if (LOW == value)
  {
//    Serial.print("Switch -> LOW\n");
    return Control::Disabled;
  }
  else
  {
//    Serial.print("Switch -> HIGH\n");
    return Control::Enabled;
  }
}

class UltraSound
{
public:
  UltraSound(
    uint8_t const pinTrig,
    uint8_t const pinEcho);

  double read() const;

private:
  uint8_t const m_pinTrig;
  uint8_t const m_pinEcho;
};

UltraSound::UltraSound(
  uint8_t const pinTrig,
  uint8_t const pinEcho)
  : m_pinTrig(pinTrig)
  , m_pinEcho(pinEcho)
{
  pinMode(m_pinTrig, OUTPUT);
  pinMode(m_pinEcho, INPUT);
}

double UltraSound::read() const
{
  digitalWrite(m_pinTrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(m_pinTrig, LOW);
  long pulseWidth = pulseIn(m_pinEcho, HIGH); 
  return (pulseWidth / 5.8);
}

class Led
{
public:
  Led(
    uint8_t const index,
    Adafruit_NeoPixel & leds);

  void set(
    uint8_t const red,
    uint8_t const green,
    uint8_t const blue);

  bool write();

private:
  uint8_t const index;
  Adafruit_NeoPixel & leds;
  uint8_t red;
  uint8_t green;
  uint8_t blue;
  uint8_t newRed;
  uint8_t newGreen;
  uint8_t newBlue;
  bool hasNew;       
};

Led::Led(
  uint8_t const index,
  Adafruit_NeoPixel & leds)
: index(index)
, leds(leds)
, red(0)
, green(0)
, blue(0)
, newRed(0)
, newGreen(0)
, newBlue(0)
, hasNew(true)
{
}

void Led::set(
  uint8_t const redValue,
  uint8_t const greenValue,
  uint8_t const blueValue) {
  if (red != redValue || green != greenValue || blue != blueValue) {
    hasNew = true;
    newRed = redValue;
    newGreen = greenValue;
    newBlue = blueValue;
  }
}

bool Led::write()
{
  if (hasNew) {
    leds.setPixelColor(index, newRed, newGreen, newBlue);
    red = newRed;
    green = newGreen;
    blue = newBlue;
    hasNew = false;
    
    return true;
  }
  return false;
}

Motor MOTOR1(MOTOR1_PWM_PIN, MOTOR1_IN1_PIN, MOTOR1_IN2_PIN);
Motor MOTOR2(MOTOR2_PWM_PIN, MOTOR2_IN1_PIN, MOTOR2_IN2_PIN);
//Switch SWITCH(8);
UltraSound US(US_TRIG_PIN, US_ECHO_PIN);
MFRC522 RFID(RFID_SDA_PIN, RFID_RST_PIN);  // Create MFRC522 instance
Adafruit_NeoPixel LEDS = Adafruit_NeoPixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
Led LED1(0, LEDS);
Led LED2(1, LEDS);

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

  uint8_t green = map(distance, 0, 2000, 0, MAX_LIGHT);
  uint8_t blue  = map(distance, 0, 2000, 0, MAX_LIGHT);
  uint8_t red   = map(distance, 0, 2000, MAX_LIGHT, 0);

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
