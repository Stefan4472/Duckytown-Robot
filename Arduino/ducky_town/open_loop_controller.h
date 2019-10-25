#ifndef OPEN_LOOP_CONTROLLER_H
#define OPEN_LOOP_CONTROLLER_H

#include "odometry_interface.h"
#include "wheel_interface.h"

class OpenLoopController
{
  private:
    float cmdLeftSpeed, cmdRightSpeed;

    // Distance travelled on left and right wheels so far this command.
    bool distSpecified;
    long distLeft, distRight;
    long targetDistLeft, targetDistRight;
 
    
    
  public:
    void update(OdometryInterface* odometry, WheelInterface* wheels);
    void commandStraight(float cmPerSec, WheelInterface* wheels);
    void commandRightTurn(float cmPerSec, float turnRadius, WheelInterface* wheels);
    void commandLeftTurn(float cmPerSec, float turnRadius, WheelInterface* wheels);
};

#endif
