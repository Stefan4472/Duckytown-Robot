#include "closed_loop_controller.h"

// Spring and Damper constants for the theta PD controller.
#define K_ROT 3.85
#define B_ROT 0

void ClosedLoopController::init()
{
  updatePeriodMs = 100;
  lastUpdateMs = 0;
}

void ClosedLoopController::update(OdometryInterface* odometry, WheelInterface* wheels, LightsInterface* lights)
{
  if (!finished && millis() - lastUpdateMs >= updatePeriodMs)
  {
    float theta_error = targetY;
    float theta_dot_error = odometry->dTheta;

    float theta_ddot = -K_ROT * theta_error - B_ROT * theta_dot_error;
    float SPEEDLIM = 15.0;
    wheels->commandSpeeds(SPEEDLIM - theta_ddot, SPEEDLIM + theta_ddot);
  }
}

void ClosedLoopController::commandPosition(float x, float y, float theta)
{
  targetX = x;
  targetY = y;
  targetTheta = theta;
  finished = false;
}

void ClosedLoopController::cancel(LightsInterface* lights)
{
  if (!finished)
  {
    finished = true;
  }
}
