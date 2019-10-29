#ifndef CLOSED_LOOP_CONTROLLER_H
#define CLOSED_LOOP_CONTROLLER_H

#include "icontroller.h"
#include "odometry_interface.h"
#include "wheel_interface.h"

#define K 1.0
#define B 1.0

class ClosedLoopController : public IController
{
  private:
    float targetX, targetY, targetTheta;

  public:
    void update(OdometryInterface* odometry, WheelInterface* wheels);
    void commandPosition(float x, float y, float theta);
};

#endif
