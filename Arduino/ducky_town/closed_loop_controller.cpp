#include "closed_loop_controller.h"

// Spring and Damper constants for the theta PD controller.
#define K_ROT 0.7 //3.85
#define B_ROT 0

void ClosedLoopController::init()
{
  updatePeriodMs = 100;
  lastUpdateMs = 0;
}

void ClosedLoopController::update(OdometryInterface* odometry, WheelInterface* wheels, LightsInterface* lights)
{
  unsigned long curr_time = millis();
  if (!finished && curr_time - lastUpdateMs >= updatePeriodMs)
  {
    float theta_dot_error = odometry->dTheta;

    float theta_ddot = -K_ROT * thetaErr - B_ROT * theta_dot_error;
    wheels->commandSpeeds(desiredSpeed - theta_ddot, desiredSpeed + theta_ddot);
    lastUpdateMs = curr_time;
  }
}

void ClosedLoopController::setSpeed(float speed)
{
  desiredSpeed = speed;
}

void ClosedLoopController::commandThetaError(float thetaError)
{
  thetaErr = thetaError;
  finished = false;
}

void ClosedLoopController::cancel(LightsInterface* lights)
{
  if (!finished)
  {
    finished = true;
  }
}
