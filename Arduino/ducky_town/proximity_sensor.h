#ifndef PROXIMITY_SENSOR_H
#define PROXIMITY_SENSOR_H

#include "wheel_interface.h"
class ProximitySensor
{
  private:
    int pin;
    
  public:
    // Always leave this distance ahead of vehicle.
    int minFollowDistanceCm = 25.0;

    // Inits Ping sensor to the given pin.
    void init(int pin);
    // Gets distance to nearest object.
    float getDistCm();
    // Applies a PWM override if necessary to
    // maintain stopping distance.
    void runCollisionAvoidance(float dx, WheelInterface* wheels);
};

#endif
