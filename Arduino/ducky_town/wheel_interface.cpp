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

int WheelInterface::leftPWMFromSpeed(float cmPerSec)  // TODO: USE NEW TRACK PWMS
{
//  return 6.24 * cmPerSec + 83.16;
//  return (int) (5.98 * cmPerSec + 83.16);
return (int) (6.02 * cmPerSec + 83.16);
}

int WheelInterface::rightPWMFromSpeed(float cmPerSec)  // TODO: DOES THIS WORK FOR NEGATIVE VALUES?
{
  return (int) (6.13 * cmPerSec + 83.2);
}

float WheelInterface::leftSpeedFromPWM(int pwm)
{
  // Return 0 if pwm is too low to move the robot.
  if (abs(pwm) < 85)
  {
    return 0.0;
  }
//  return ((float) pwm - 83.16) / 5.98;
return ((float) pwm - 83.16) / 6.02;
//  return 0.160 * pwm - 13.33;
}

float WheelInterface::rightSpeedFromPWM(int pwm)
{
  // Return 0 if pwm is too low to move the robot.
  if (abs(pwm) < 85)
  {
    return 0.0;
  }
  return ((float) pwm - 83.2) / 6.13;
//  return 0.163 * pwm - 13.56;
}

bool WheelInterface::init()
{
  this->motorShield.init();
  return !this->motorShield.getFault();
}

bool WheelInterface::commandPWMs(int leftPWM, int rightPWM)
{ 
  // Serial.println("currPWM " + String(currLPWM) + ", " + String(currRPWM));
  // Serial.println("commanded " + String(leftPWM) + ", " + String(rightPWM));
  
  lastCommandedLeftPWM = boundLeftPWM(leftPWM);
  lastCommandedRightPWM = boundRightPWM(rightPWM);

  // If override is currently on, do nothing for now.
  // The commanded PWMs will be applied later.
  if (overrideOn)
  {
    return true;
  }
  else 
  {
    this->motorShield.setM1Speed(lastCommandedLeftPWM);
    this->motorShield.setM2Speed(lastCommandedRightPWM);

    currLeftPWM = lastCommandedLeftPWM;
    currRightPWM = lastCommandedRightPWM;

    return !this->motorShield.getFault();
  }
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

void WheelInterface::startSpeedOverride(float cmPerSec)
{
  overrideOn = true;

  currLeftPWM = boundLeftPWM(leftPWMFromSpeed(cmPerSec));
  currRightPWM = boundRightPWM(rightPWMFromSpeed(cmPerSec));

  this->motorShield.setM1Speed(currLeftPWM);
  this->motorShield.setM2Speed(currRightPWM);
}

void WheelInterface::stopSpeedOverride()
{
  overrideOn = false;
  
  this->motorShield.setM1Speed(lastCommandedLeftPWM);
  this->motorShield.setM2Speed(lastCommandedRightPWM);


}