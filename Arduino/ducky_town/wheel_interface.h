#ifndef WHEEL_INTERFACE_H
#define WHEEL_INTERFACE_H

#include "DualMC33926MotorShield.h"

class WheelInterface
{
  private:
    // MotorShield
    DualMC33926MotorShield motorShield;
    // Convert desired speed to PWM for left wheel
    float leftSpeedToPWM(float cmPerSec);
    // Convert desired speed to PWM for right wheel
    float rightSpeedToPWM(float cmPerSec);
    
  public:
    void init();
    // Sets raw PWM inputs to left and right motors
    bool commandPWMs(int leftPWM, int rightPWM);
    bool commandStraightSpeed(float cmPerSec);
    bool commandSpeeds(float leftCmPerSec, float rightCmPerSec);
};


#endif
