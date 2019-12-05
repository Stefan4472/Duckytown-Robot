#ifndef OPEN_LOOP_CONTROLLER_H
#define OPEN_LOOP_CONTROLLER_H

#include "icontroller.h"
#include "odometry_interface.h"
#include "wheel_interface.h"
#include "lights_interface.h"

enum OpenControlType
{
  NONE,
  STRAIGHT,
  LEFT,
  RIGHT
};

class OpenLoopController : public IController
{
  private:
    // Target distance for this command. Set to zero if
    // no distance is specified.
    float targetDistance;
    // Distance travelled so far this command.
    float distanceTravelled;
    // Last type of control applied.
    OpenControlType lastControl = OpenControlType::NONE;

  public:  // TODO: CAN THIS HANDLE NEGATIVE SPEEDS?
    void init();
    void update(OdometryInterface* odometry, WheelInterface* wheels, LightsInterface* lights);
    // Provide target == 0.0 for no target
    void commandStraight(float cmPerSec, float targetDistCm, WheelInterface* wheels);  // TODO: A COMMANDPOSITION FUNCTION
    void commandRightTurn(float cmPerSec, float turnRadius, float targetRad, WheelInterface* wheels, LightsInterface* lights);
    void commandLeftTurn(float cmPerSec, float turnRadius, float targetRad, WheelInterface* wheels, LightsInterface* lights);
    void cancel(LightsInterface* lights);
};

#endif
