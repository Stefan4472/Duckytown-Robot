#include "wheel_interface.h"

float WheelInterface::leftSpeedToPWM(float cmPerSec)
{
  
}

float WheelInterface::rightSpeedToPWM(float cmPerSec)
{
  
}

void WheelInterface::init()
{
  this->motorShield.init();
  if (this->motorShield.getFault())
  {
    Serial.println("Fault");
  }
}

bool WheelInterface::commandPWMs(int leftPWM, int rightPWM)
{
  Serial.println("WheelInterface received command");
  if (this->motorShield.getFault())
  {
    Serial.println("Fault");
  }
  this->motorShield.setM1Speed(leftPWM);
  this->motorShield.setM2Speed(rightPWM);
  if (this->motorShield.getFault())
  {
    Serial.println("Fault");
  }
}

bool WheelInterface::commandStraightSpeed(float cmPerSec)
{
  
}

bool WheelInterface::commandSpeeds(float leftCmPerSec, float rightCmPerSec)
{
  
}
