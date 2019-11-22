#include "lights.h"
#include "Arduino.h"

void LightsInterface::init(int leftPin, int rightPin)
{
  this->leftPin = leftPin;
  this->rightPin = rightPin;
}

void LightsInterface::update()
{
  // Brake is on: nothing to update.
  if (brakeOn)
  {

  }
  // Left blinker on.
  else if (blinkerState == BlinkerState::BLINKING_LEFT)
  {
    unsigned long curr_time = millis();
    // Toggle the light. Make sure to handle the case
    // where the brake "covered" up the blinker.
    while (curr_time - lastBlinkMs >= BLINK_PERIOD_MS)
    {
      digitalWrite(leftPin, !digitalRead(leftPin));
    }
  }
  // Right blinker on.
  else if (blinkerState == BlinkerState::BLINKING_RIGHT)
  {
    unsigned long curr_time = millis();
    // Toggle the light. Make sure to handle the case
    // where the brake "covered" up the blinker.
    while (curr_time - lastBlinkMs >= BLINK_PERIOD_MS)
    {
      digitalWrite(rightPin, !digitalRead(rightPin));
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

}

void LightsInterface::stopRightBlinker()
{

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