#include "open_loop_controller.h"

void OpenLoopController::update(OdometryInterface* odometry, WheelInterface* wheels)
{
//  Serial.println("OpenLoop running update()");
//  Serial.println("currDist = " + String(odometry->distTravelled));
//  Serial.println("prevDist = " + String(odometry->prevDistTravelled));
  // Update tracked distance travelled.
  distanceTravelled += odometry->distTravelled - odometry->prevDistTravelled;
//  Serial.println("distanceTravelled now " + String(distanceTravelled));
  // Stop the motors if we've travelled the desired distance.
  if (targetDistance != 0.0 && abs(distanceTravelled) >= abs(targetDistance))
  {
//    Serial.println("Finished, reached target " + String(targetDistance));
    wheels->commandPWMs(0, 0);
    finished = true;
  }
}

void OpenLoopController::commandStraight(float cmPerSec, float targetDistCm, WheelInterface* wheels)
{
  wheels->commandStraightSpeed(cmPerSec);
  this->targetDistance = targetDistCm;
  finished = false;
}

void OpenLoopController::commandRightTurn(float cmPerSec, float turnRadius, float targetRad, WheelInterface* wheels)
{
//  Serial.println("Openloop received command to turn right");
  float left_speed = cmPerSec * (turnRadius + WHEEL_BASE_CM / 2.0) / turnRadius;
  float right_speed = cmPerSec * (turnRadius - WHEEL_BASE_CM / 2.0) / turnRadius;
  // Calculate desired straight-line distance (0.0 = no target).
  targetDistance = targetRad * WHEEL_CIRCUMFERENCE_CM;
  wheels->commandSpeeds(left_speed, right_speed);
  finished = false;
}

void OpenLoopController::commandLeftTurn(float cmPerSec, float turnRadius, float targetRad, WheelInterface* wheels)
{
//  Serial.println("Openloop received command to turn left");
  float left_speed = cmPerSec * (turnRadius - WHEEL_BASE_CM / 2.0) / turnRadius;
  float right_speed = cmPerSec * (turnRadius + WHEEL_BASE_CM / 2.0) / turnRadius;
  // Calculate desired straight-line distance (0.0 = no target).
  targetDistance = targetRad * WHEEL_CIRCUMFERENCE_CM;
//  Serial.print("Calculated speeds ");
//  Serial.print(left_speed);
//  Serial.print(" - ");
//  Serial.print(right_speed);
//  Serial.println();
  wheels->commandSpeeds(left_speed, right_speed);
  finished = false;
}
