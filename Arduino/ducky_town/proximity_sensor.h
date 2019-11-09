#ifndef PROXIMITY_SENSOR_H
#define PROXIMITY_SENSOR_H

// #include "wheel_interface.h"

class ProximitySensor
{
  private:
    int pin;
    
  public:
    // Inits Ping sensor to the given pin.
    void init(int pin);
    // Gets distance to nearest object.
    float getDistCm();
};

#endif
