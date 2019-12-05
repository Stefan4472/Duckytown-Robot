#ifndef CLOSED_LOOP_CONTROLLER_H
#define CLOSED_LOOP_CONTROLLER_H

#include "icontroller.h"
#include "odometry_interface.h"
#include "wheel_interface.h"
#include "lights_interface.h"

class ClosedLoopController : public IController
{
  private:
    float targetX, targetY, targetTheta;
    float speedLim;

  public:
    void init();
    void update(OdometryInterface* odometry, WheelInterface* wheels, LightsInterface* lights);
    void commandPosition(float x, float y, float theta);
    void cancel(LightsInterface* lights);
};

#endif
