#ifndef CLOSED_LOOP_CONTROLLER_H
#define CLOSED_LOOP_CONTROLLER_H

#include "icontroller.h"
#include "odometry_interface.h"
#include "wheel_interface.h"

class ClosedLoopController : public IController
{
  private:
    float targetX, targetY, targetTheta;

  public:
    void update(OdometryInterface* odometry, WheelInterface* wheels);
    void commandPosition(float x, float y, float theta);
    void doATurn(float radius, float velocity, bool left, long startTime);
    void (*onConvergedFcn)() = NULL;
};

#endif
