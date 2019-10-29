#include "wheel_interface.h"

int WheelInterface::boundLeftPWM(int pwm)
{
  if (pwm > maxLeftPWM)
  {
    return maxLeftPWM;
  }
  else if (pwm < minLeftPWM)
  {
    return minLeftPWM;
  }
  else
  {
    return pwm;
  }
}

int WheelInterface::boundRightPWM(int pwm)
{
  if (pwm > maxRightPWM)
  {
    return maxRightPWM;
  }
  else if (pwm < minRightPWM)
  {
    return minRightPWM;
  }
  else
  {
    return pwm;
  }
}

int WheelInterface::leftPWMFromSpeed(float cmPerSec)
{
  return 6.24 * cmPerSec + 83.16;
}

int WheelInterface::rightPWMFromSpeed(float cmPerSec)  // TODO: DOES THIS WORK FOR NEGATIVE VALUES?
{
  return 6.13 * cmPerSec + 83.2;
}

float WheelInterface::leftSpeedFromPWM(uint16_t pwm)
{
  // Return 0 if pwm is too low to move the robot.
  if (abs(pwm) < 85)
  {
    return 0.0;
  }
  return 0.160 * pwm - 13.33;
}

float WheelInterface::rightSpeedFromPWM(uint16_t pwm)
{
  // Return 0 if pwm is too low to move the robot.
  if (abs(pwm) < 85)
  {
    return 0.0;
  }
  return 0.163 * pwm - 13.56;
}

bool WheelInterface::init()
{
  this->motorShield.init();
  return !this->motorShield.getFault();
}

bool WheelInterface::commandPWMs(int leftPWM, int rightPWM)
{ 
  currLPWM  = boundLeftPWM(leftPWM);
  currRPWM  = boundRightPWM(rightPWM);
  Serial.print("WheelInterface::commandPWMs: left=");
  Serial.print(currLPWM);
  Serial.print(" right=");
  Serial.print(currRPWM);
  Serial.println();
  this->motorShield.setM1Speed(currLPWM);
  this->motorShield.setM2Speed(currRPWM);
  return !this->motorShield.getFault();
}

bool WheelInterface::commandStraightSpeed(float cmPerSec)
{
  return commandPWMs(leftPWMFromSpeed(cmPerSec), rightPWMFromSpeed(cmPerSec));
}

bool WheelInterface::commandSpeeds(float leftCmPerSec, float rightCmPerSec)
{
  return commandPWMs(leftPWMFromSpeed(leftCmPerSec), rightPWMFromSpeed(rightCmPerSec));
}

void WheelInterface::setSpeedLimit(float cmPerSec)
{
  cmPerSec = abs(cmPerSec);
  this->maxRightPWM = rightPWMFromSpeed(cmPerSec);
  this->maxLeftPWM = leftPWMFromSpeed(cmPerSec);
  this->minRightPWM = rightPWMFromSpeed(-cmPerSec);
  this->minLeftPWM = leftPWMFromSpeed(-cmPerSec);
}
