#include "wheel_interface.h"

int WheelInterface::boundPWM(int pwm)
{
  if (pwm > MAX_PWM)
  {
    return MAX_PWM;
  }
  else if (pwm < MIN_PWM)
  {
    return MIN_PWM;
  }
  else
  {
    return pwm;
  }
}

uint16_t WheelInterface::leftSpeedToPWM(float cmPerSec)
{
  if (cmPerSec > 0)
  {
    
  }
  else
  {
    
  }
}

uint16_t WheelInterface::rightSpeedToPWM(float cmPerSec)
{
  if (cmPerSec > 0)
  {
    
  }
  else
  {
    
  }
}

void WheelInterface::init()  // TODO: MAKE BOOLEAN
{
  this->motorShield.init();
//  return !this->motorShield.getFault();
}

bool WheelInterface::commandPWMs(int leftPWM, int rightPWM)  // TODO: BOUND TO SPEED LIMIT?
{ 
  this->motorShield.setM1Speed(boundPWM(leftPWM));
  this->motorShield.setM2Speed(boundPWM(rightPWM));
  return !this->motorShield.getFault();
}

bool WheelInterface::commandStraightSpeed(float cmPerSec)
{
  
  return !this->motorShield.getFault();
}

bool WheelInterface::commandSpeeds(float leftCmPerSec, float rightCmPerSec)
{
  return !this->motorShield.getFault();
}

void WheelInterface::setSpeedLimit(float cmPerSec)
{
  // TODO: BOUND TO MAX/MIN MOTOR LIMITS
}
