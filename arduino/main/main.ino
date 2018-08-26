#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <MFRC522.h>

// RFID needs 3.3V
uint8_t const RFID_RST_PIN = 10;
uint8_t const RFID_SDA_PIN = 9;

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

uint8_t const VOLTAGE_PIN = A3;
uint8_t const VOLTAGE_RATIO = 11;

uint8_t const MIN_PWM = 50;
uint8_t const MAX_PWM = 255;
bool isClockwise = false;
bool IncreaseSpeed = true;
uint8_t step = 5;

byte const KEY1[4] = {0xB7,0x84,0x20,0xD9};
byte const KEY2[4] = {0x60,0x79,0xFA,0xA3};

enum class Direction
{
  Forward,
  Reverse
};

enum class Status
{
  Stopped,
  Running
};

enum class Control
{
  Disabled,
  Enabled  
};

enum class Update
{
  Full,
  Partial,
  None
};

class Motor
{
public:
  Motor(
    uint8_t const pinPWM,
    uint8_t const pinIN1,
    uint8_t const pinIN2);

  void disable();
  void enable();
  void toggle();

  void update();
  void setSpeedRange(
    uint8_t const min,
    uint8_t const max);
  void setSpeed(uint8_t const valuePWM);
  Update increaseSpeed(uint8_t const delta);
  Update decreaseSpeed(uint8_t const delta);

  void setDirection(Direction const direction);

  void switchDirection();
  
private:
  void updateDirection();
  void updateSpeed();
  uint8_t const m_pinPWM;
  uint8_t const m_pinIN1;
  uint8_t const m_pinIN2;
  uint8_t m_valuePWM;
  uint8_t m_minPWM;
  uint8_t m_maxPWM;
  Direction m_direction;
  Status m_status;
  Control m_control;
};

Motor::Motor(
  uint8_t const pinPWM,
  uint8_t const pinIN1,
  uint8_t const pinIN2)
  : m_pinPWM(pinPWM)
  , m_pinIN1(pinIN1)
  , m_pinIN2(pinIN2)
  , m_valuePWM(0)
  , m_minPWM(0)
  , m_maxPWM(MAX_PWM)
  , m_direction(Direction::Forward)
  , m_status(Status::Stopped)
  , m_control(Control::Disabled)
{
  pinMode(m_pinPWM, OUTPUT);
  pinMode(m_pinIN1, OUTPUT);
  pinMode(m_pinIN2, OUTPUT);
}

void Motor::enable()
{
  if (Control::Disabled == m_control)
  {
    m_control = Control::Enabled;
    updateDirection();
    updateSpeed();
  }
}

void Motor::disable()
{
  if (Control::Enabled == m_control)
  {
    analogWrite(m_pinPWM, 0);
    m_control = Control::Disabled;
  }
}

void Motor::toggle()
{
  if (Control::Enabled == m_control)
  {
    disable();
  }
  else
  {
    enable();
  }
}

void Motor::update()
{
  updateDirection();
  updateSpeed();
}

void Motor::setSpeedRange(
  uint8_t const min,
  uint8_t const max)
{
  if (min <= max)
  {
    m_minPWM = min;
    m_maxPWM = max;
  }
  else
  {
    // we are not allowed exceptions so make things work
    m_minPWM = max;
    m_maxPWM = min;
  }
  if (Status::Running == m_status)
  {
    if (m_valuePWM < m_minPWM)
    {
      m_valuePWM = m_minPWM;
    }
    else if (m_valuePWM > m_maxPWM)
    {
      m_valuePWM = m_maxPWM;
    }
  }
}

void Motor::setSpeed(uint8_t const valuePWM)
{
  if (valuePWM > m_maxPWM)
  {
    m_valuePWM = m_maxPWM;
  }
  else if (valuePWM < m_minPWM)
  {
    m_valuePWM = m_minPWM;
  }
  else
  {
    m_valuePWM = valuePWM;
  }
  if (m_valuePWM != 0)
  {
    m_status = Status::Running;
  }
}

Update Motor::increaseSpeed(uint8_t const delta)
{
//  Serial.print("increaseSpeed(");
//  Serial.print(delta);
//  Serial.print(")\n");
  Update update = Update::None;
  if (0 != delta)
  {
    if ((m_valuePWM + delta)  > m_maxPWM)
    {
      m_valuePWM = m_maxPWM;
      update = Update::Partial;
    }
    else
    {
      m_valuePWM += delta;
      update = Update::Full;
    }
    m_status = Status::Running;
  }
  return update;
}

Update Motor::decreaseSpeed(uint8_t const delta)
{
//  Serial.print("deccreaseSpeed(");
//  Serial.print(delta);
//  Serial.print(")\n");
  Update update = Update::None;
  if (0 != delta)
  {
    if ((m_valuePWM - delta) < m_minPWM)
    {
      m_valuePWM = m_minPWM;
      update = Update::Partial;
    }
    else
    {
      m_valuePWM -= delta;
      update = Update::Full;
    }
    if (0 == m_valuePWM)
    {
      m_status = Status::Stopped;
    }
  }
  return update;
}

void Motor::setDirection(Direction const direction)
{
  m_direction = direction;
}

void Motor::switchDirection()
{
  switch (m_direction)
  {
    case Direction::Forward:
    {
      m_direction = Direction::Reverse;
      break;
    }
    case Direction::Reverse:
    {
      m_direction = Direction::Forward;
      break;
    }
  }
}

void Motor::updateDirection()
{
  if (Control::Enabled == m_control)
  {
    switch (m_direction)
    {
      case Direction::Forward:
      {
//        Serial.print("Forward\n");
        digitalWrite(m_pinIN1, HIGH);
        digitalWrite(m_pinIN2, LOW);
        break;
      }
      case Direction::Reverse:
      {
//        Serial.print("Reverse\n");
        digitalWrite(m_pinIN1, LOW);
        digitalWrite(m_pinIN2, HIGH);
        break;
      }
    }
  }
}

void Motor::updateSpeed()
{
  if (Control::Enabled == m_control)
  {
//    Serial.print("Speed:");
//    Serial.print(m_valuePWM);
//    Serial.print("\n");
    analogWrite(m_pinPWM, m_valuePWM);
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

Motor MOTOR1(MOTOR1_PWM_PIN, MOTOR1_IN1_PIN, MOTOR1_IN2_PIN);
Motor MOTOR2(MOTOR2_PWM_PIN, MOTOR2_IN1_PIN, MOTOR2_IN2_PIN);
//Switch SWITCH(8);
UltraSound US(US_TRIG_PIN, US_ECHO_PIN);
MFRC522 RFID(RFID_SDA_PIN, RFID_RST_PIN);  // Create MFRC522 instance
Adafruit_NeoPixel LEDS = Adafruit_NeoPixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup()
{
  Serial.begin(9600);
  MOTOR1.setSpeedRange(MIN_PWM, MAX_PWM);
  MOTOR2.setSpeedRange(MIN_PWM, MAX_PWM);
//  MOTOR1.enable();
  MOTOR1.setSpeed(MIN_PWM);
  MOTOR2.setSpeed(MIN_PWM);
  //MOTOR2.enable();
  //MOTOR2.increaseSpeed(step * 4);
  Serial.begin(9600);
  SPI.begin(); // Init SPI bus
  RFID.PCD_Init(); // Init MFRC522
  LEDS.begin();
}

long LAST_TOGGLE = 0;
bool TOGGLE = false;
long NOW = 0;

void loop()
{
  double distance = US.read();
  if (distance > 2000)
  {
    distance = 2000;
  }

  uint8_t newSpeed1 = map(distance, 0, 2000, MAX_PWM, MIN_PWM);
  uint8_t newSpeed2 = map(distance, 0, 2000, MIN_PWM, MAX_PWM);
  float voltage = analogRead(VOLTAGE_PIN) * VOLTAGE_RATIO * 5 / 1024.0;
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print(" mm ; speed1: ");
  Serial.print(newSpeed1);
  Serial.print(" ; speed2: ");
  Serial.print(newSpeed2);
  Serial.print(" ; voltage: ");
  Serial.print(voltage);
  Serial.print(" V");
  Serial.print("\n");
  MOTOR1.setSpeed(newSpeed1);
  MOTOR2.setSpeed(newSpeed2);

  MOTOR1.update();
  MOTOR2.update();

  
  uint8_t green = map(distance, 0, 2000, 0, MAX_LIGHT);
  uint8_t blue  = map(distance, 0, 2000, 0, MAX_LIGHT);
  uint8_t red   = map(distance, 0, 2000, MAX_LIGHT, 0);

  LEDS.setPixelColor(0, red, green, 0);
  LEDS.setPixelColor(1, red, 0, blue);
  LEDS.show();

  delay(20);

//  if (IncreaseSpeed)
//  {
//    if (Update::Partial == MOTOR1.increaseSpeed(step))
//    {
////      delay(4000);
//      IncreaseSpeed = false;
//    }
//  }
//  else
//  {
//    if (Update::Partial == MOTOR1.decreaseSpeed(step))
//    {
//      IncreaseSpeed = true;
//      MOTOR1.switchDirection();
//    }
//  }

  if (RFID.PICC_IsNewCardPresent() and RFID.PICC_ReadCardSerial())
  {
    Serial.print(F("The NUID tag is: "));
    printHex(RFID.uid.uidByte, RFID.uid.size);
    Serial.println();

    if (strncmp(RFID.uid.uidByte, KEY1, RFID.uid.size) == 0) 
    {
      Serial.println("KEY1 detected!");
      MOTOR1.toggle();
    }
    else if (strncmp(RFID.uid.uidByte, KEY2, RFID.uid.size) == 0) 
    {
      Serial.println("KEY2 detected!");
      MOTOR2.toggle();
    }
  }
  
//  if (SWITCH.read() == Control::Enabled)
//  {
//    if (not TOGGLE)
//    {
//      if (NOW - LAST_TOGGLE > 1000)
//      {
//        Serial.print("Toggle\n");
//        MOTOR1.toggle();
//        MOTOR2.toggle();
//        TOGGLE = true;
//        LAST_TOGGLE = NOW;
//      }
//    }
//  }
//  else
//  {
//    TOGGLE = false;
//  }
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
