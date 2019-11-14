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
  return (int) (89.81 + cmPerSec * 6.024);
}

int WheelInterface::rightPWMFromSpeed(float cmPerSec)  // TODO: DOES THIS WORK FOR NEGATIVE VALUES?
{
  return (int) (88.39 + cmPerSec * 5.952);
}

float WheelInterface::leftSpeedFromPWM(int pwm)
{
  // Return 0 if pwm is too low to move the robot.
  if (abs(pwm) < 90)
  {
    return 0.0;
  }
  return 0.166 * pwm - 14.91;
}

float WheelInterface::rightSpeedFromPWM(int pwm)
{
  // Return 0 if pwm is too low to move the robot.
  if (abs(pwm) < 89)
  {
    return 0.0;
  }
  return 0.168 * pwm - 14.85;
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
//    Serial.println("Commanding PWMs " + String(lastCommandedLeftPWM) + ", " + String(lastCommandedRightPWM));

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
//  Serial.println("Commanding speeds " + String(leftCmPerSec) + ", " + String(rightCmPerSec));
  int left_pwm = leftPWMFromSpeed(leftCmPerSec);
  int right_pwm = rightPWMFromSpeed(rightCmPerSec);

  // Handle PWM over speed limit: bring down to speed limit and keep the ratio constant.
//  if (left_pwm > maxLeftPWM)
//  {
//    float r_to_l = right_pwm * 1.0 / left_pwm;
//    left_pwm = maxLeftPWM;
//    right_pwm = left_pwm * r_to_l;
//  }3
//  if (right_pwm > maxRightPWM)
//  {
//   float l_to_r = left_pwm * 1.0 / right_pwm;
//   right_pwm = maxRightPWM;
//   left_pwm = right_pwm * l_to_r; 
//  }
  
  return commandPWMs(left_pwm, right_pwm);
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
