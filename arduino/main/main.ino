uint8_t const MAX_PWM = 255;
bool isClockwise = false;
bool IncreaseSpeed = true;
uint8_t const MIN_PWM = 50;
uint8_t step = 5;

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

Motor MOTOR2(9, 6, 7);
Motor MOTOR1(10, 4, 5);
Switch SWITCH(8);
UltraSound US(2, 3);

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

  uint8_t newSpeed = map(distance, 0, 2000, MIN_PWM, MAX_PWM);
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print(" mm ; speed: ");
  Serial.print(newSpeed);
  Serial.print("\n");
  MOTOR2.setSpeed(newSpeed);

  MOTOR1.update();
  MOTOR2.update();

  delay(20);

  if (IncreaseSpeed)
  {
    if (Update::Partial == MOTOR1.increaseSpeed(step))
    {
//      delay(4000);
      IncreaseSpeed = false;
    }
  }
  else
  {
    if (Update::Partial == MOTOR1.decreaseSpeed(step))
    {
      IncreaseSpeed = true;
      MOTOR1.switchDirection();
    }
  }
  NOW = millis();
  if (SWITCH.read() == Control::Enabled)
  {
    if (not TOGGLE)
    {
      if (NOW - LAST_TOGGLE > 1000)
      {
        Serial.print("Toggle\n");
        MOTOR2.toggle();
        TOGGLE = true;
        LAST_TOGGLE = NOW;
      }
    }
  }
  else
  {
    TOGGLE = false;
  }
  delay(400);
}
