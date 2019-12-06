#include "lights_interface.h"
#include "Arduino.h"

void LightsInterface::init(int leftPin, int rightPin)
{
  this->leftPin = leftPin;
  this->rightPin = rightPin;
  pinMode(leftPin, OUTPUT);
  pinMode(rightPin, OUTPUT);
}

void LightsInterface::update(float currSpeed)
{
  bool activate_brake = (currSpeed == 0.0 || currSpeed < (lastSpeed * 0.8));
  if (activate_brake && !brakeOn)
  {
    startBrakeLight();
  }
  else if (!activate_brake && brakeOn)
  {
    stopBrakeLight();
  }
  
  lastSpeed = currSpeed;

//  Serial.println("Brake = " + String(brakeOn) + ", blinker = " + String(blinkerState));
  // Brake is on: nothing to update. The LEDs should 
  // already be on.
  if (brakeOn)
  {

  }
  // Brake off, left blinker on.
  else if (blinkerState == BlinkerState::BLINKING_LEFT)
  {
    unsigned long curr_time = millis();

    // Toggle the light.
    if (curr_time - lastBlinkMs >= BLINK_PERIOD_MS)
    {
      digitalWrite(leftPin, !digitalRead(leftPin));
      lastBlinkMs = curr_time;
    }
  }
  // Brake off, right blinker on.
  else if (blinkerState == BlinkerState::BLINKING_RIGHT)
  {
    unsigned long curr_time = millis();

    // Toggle the light. 
    if (curr_time - lastBlinkMs >= BLINK_PERIOD_MS)
    {
      digitalWrite(rightPin, !digitalRead(rightPin));
      lastBlinkMs = curr_time;
    }
  }
}

void LightsInterface::startLeftBlinker()
{
  // Turn off right blinker, unless the brake is on
  if (blinkerState == BlinkerState::BLINKING_RIGHT &&
      !brakeOn)
  {
    digitalWrite(rightPin, LOW);
  }

  blinkerState = BlinkerState::BLINKING_LEFT;
  digitalWrite(leftPin, HIGH);
  lastBlinkMs = millis();
}

void LightsInterface::stopLeftBlinker()
{
  if (blinkerState == BlinkerState::BLINKING_LEFT)
  {
    blinkerState = BlinkerState::NOT_BLINKING;
    
    // Turn off, unless brake is eganged
    if (!brakeOn)
    {
      digitalWrite(leftPin, LOW);
    }
  }  
}

void LightsInterface::startRightBlinker()
{
  // Turn off left blinker, unless the brake is on
  if (blinkerState == BlinkerState::BLINKING_LEFT &&
      !brakeOn)
  {
    digitalWrite(leftPin, LOW);
  }
  
  blinkerState = BlinkerState::BLINKING_RIGHT;
  digitalWrite(rightPin, HIGH);
  lastBlinkMs = millis();
}

void LightsInterface::stopRightBlinker()
{
  if (blinkerState == BlinkerState::BLINKING_RIGHT)
  {
    blinkerState = BlinkerState::NOT_BLINKING;
    
    // Turn off, unless brake is eganged
    if (!brakeOn)
    {
      digitalWrite(rightPin, LOW);
    }
  }  
}

void LightsInterface::startBrakeLight()
{
  brakeOn = true;
  digitalWrite(leftPin, HIGH);
  digitalWrite(rightPin, HIGH);
}

void LightsInterface::stopBrakeLight()
{
  brakeOn = false;
  digitalWrite(leftPin, LOW);
  digitalWrite(rightPin, LOW);
}

bool LightsInterface::getBrakeState()
{
  return brakeOn;
}

BlinkerState LightsInterface::getBlinkerState()
{
  return blinkerState;
}
