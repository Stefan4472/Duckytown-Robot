#ifndef OPEN_LOOP_CONTROLLER_H
#define OPEN_LOOP_CONTROLLER_H

#include "icontroller.h"
#include "odometry_interface.h"
#include "wheel_interface.h"

class OpenLoopController : public IController
{
  private:
    // Target distance for this command. Set to zero if
    // no distance is specified.
    float targetDistance;
    // Distance travelled so far this command.
    float distanceTravelled;

  public:  // TODO: CAN THIS HANDLE NEGATIVE SPEEDS?
    void update(OdometryInterface* odometry, WheelInterface* wheels);
    // Provide target == 0.0 for no target
    void commandStraight(float cmPerSec, float targetDistCm, WheelInterface* wheels);
    void commandRightTurn(float cmPerSec, float turnRadius, float targetRad, WheelInterface* wheels);
    void commandLeftTurn(float cmPerSec, float turnRadius, float targetRad, WheelInterface* wheels);
};

#endif
