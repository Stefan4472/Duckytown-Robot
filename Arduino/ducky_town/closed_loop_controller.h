#ifndef CLOSED_LOOP_CONTROLLER_H
#define CLOSED_LOOP_CONTROLLER_H

#include "icontroller.h"
#include "odometry_interface.h"
#include "wheel_interface.h"

class ClosedLoopController : public IController
{
  private:
    float targetX, targetY, targetTheta;
    float speedLim;

  public:
    void update(OdometryInterface* odometry, WheelInterface* wheels);
    void commandPosition(float x, float y, float theta);
    void (*onConvergedFcn)() = NULL;
    void setThetaControl(float speed_lim, float theta_rel);
};

#endif
