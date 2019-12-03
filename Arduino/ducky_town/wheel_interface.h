#ifndef WHEEL_INTERFACE_H
#define WHEEL_INTERFACE_H

#include "DualMC33926MotorShield.h"
#include "lights_interface.h"

#define MAX_PWM 400
#define MIN_PWM -400

// USES PINS 12, 10, 9, 8, 7, AND 4
class WheelInterface
{
  private:
    // MotorShield
    DualMC33926MotorShield motorShield;

    // PWM limits (based on speed limit and motor limits).
    // Initialize to motor limits.
    int maxRightPWM = MAX_PWM, maxLeftPWM = MAX_PWM;
    int minRightPWM = MIN_PWM, minLeftPWM = MIN_PWM;
    
    // Bounds PWM to (max, min) on left and right
    int boundLeftPWM(int pwm);
    int boundRightPWM(int pwm);
    
    // Convert desired speed to PWM for left wheel
    int leftPWMFromSpeed(float cmPerSec);
    // Convert desired speed to PWM for right wheel
    int rightPWMFromSpeed(float cmPerSec);
    
    float leftSpeedFromPWM(int pwm);
    float rightSpeedFromPWM(int pwm);
    
  public:
    int currLeftPWM, currRightPWM;  // TODO: PROVIDE CURRLEFTSPEED, CURRRIGHTSPEED INSTEAD(?)
    int lastCommandedLeftPWM, lastCommandedRightPWM;
    float speedAtLastUpdate;
    bool overrideOn = false;

    bool init();
    void update(LightsInterface* lights);
    
    // Sets raw PWM inputs to left and right motors
    bool commandPWMs(int leftPWM, int rightPWM);

    bool commandStraightSpeed(float cmPerSec);
    
    bool commandSpeeds(float leftCmPerSec, float rightCmPerSec);
    
    void setSpeedLimit(float cmPerSec);

    void startSpeedOverride(float cmPerSec);
    
    void stopSpeedOverride();
};


#endif
