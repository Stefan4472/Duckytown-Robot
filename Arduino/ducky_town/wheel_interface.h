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

    // PWM limits (based on speed limit and motor limits).
    // Initialize to motor limits.
    int16_t maxRightPWM = MAX_PWM, maxLeftPWM = MAX_PWM;
    int16_t minRightPWM = MIN_PWM, minLeftPWM = MIN_PWM;
    
    // Bounds PWM to (max, min) on left and right
    int boundLeftPWM(int pwm);
    int boundRightPWM(int pwm);
    
    // Convert desired speed to PWM for left wheel
    int leftPWMFromSpeed(float cmPerSec);
    // Convert desired speed to PWM for right wheel
    int rightPWMFromSpeed(float cmPerSec);
    
    float leftSpeedFromPWM(uint16_t pwm);
    float rightSpeedFromPWM(uint16_t pwm);
    
  public:
    int currLPWM, currRPWM;
    bool init();
    // Sets raw PWM inputs to left and right motors
    bool commandPWMs(int leftPWM, int rightPWM);
    bool commandStraightSpeed(float cmPerSec);
    bool commandSpeeds(float leftCmPerSec, float rightCmPerSec);
    void setSpeedLimit(float cmPerSec);
};


#endif
