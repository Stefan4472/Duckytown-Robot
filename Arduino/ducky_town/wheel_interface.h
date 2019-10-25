#ifndef WHEEL_INTERFACE_H
#define WHEEL_INTERFACE_H

#include "DualMC33926MotorShield.h"

#define MAX_PWM 400
#define MIN_PWM -400

class WheelInterface
{
  private:
    // MotorShield
    DualMC33926MotorShield motorShield;

    int boundPWM(int pwm);
    
    // Convert desired speed to PWM for left wheel
    uint16_t leftSpeedToPWM(float cmPerSec);
    // Convert desired speed to PWM for right wheel
    uint16_t rightSpeedToPWM(float cmPerSec);
    
    uint16_t rightAbsPWMLimit, leftAbsPWMLimit;
    
  public:
    void init();
    // Sets raw PWM inputs to left and right motors
    bool commandPWMs(int leftPWM, int rightPWM);
    bool commandStraightSpeed(float cmPerSec);
    bool commandSpeeds(float leftCmPerSec, float rightCmPerSec);
    void setSpeedLimit(float cmPerSec);
};


#endif
