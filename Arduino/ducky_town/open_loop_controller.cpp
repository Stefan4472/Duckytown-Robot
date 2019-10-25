#include "open_loop_controller.h"

void OpenLoopController::update(OdometryInterface* odometry, WheelInterface* wheels)
{
  
}

void OpenLoopController::commandStraight(float cmPerSec, WheelInterface* wheels)
{
  wheels->commandStraightSpeed(cmPerSec);
}

void OpenLoopController::commandRightTurn(float cmPerSec, float turnRadius, WheelInterface* wheels)
{
  float left_speed = cmPerSec * (turnRadius + WHEEL_BASE_CM / 2.0) / turnRadius;
  float right_speed = cmPerSec * (turnRadius - WHEEL_BASE_CM / 2.0) / turnRadius;
  wheels->commandSpeeds(left_speed, right_speed);
}

void OpenLoopController::commandLeftTurn(float cmPerSec, float turnRadius, WheelInterface* wheels)
{
  Serial.println("Openloop received command to turn left");
  Serial.print(cmPerSec);
  Serial.print(" - ");
  Serial.print(turnRadius);
  Serial.println();
  float left_speed = cmPerSec * (turnRadius - WHEEL_BASE_CM / 2.0) / turnRadius;
  float right_speed = cmPerSec * (turnRadius + WHEEL_BASE_CM / 2.0) / turnRadius;
  Serial.print("Calculated speeds ");
  Serial.print(left_speed);
  Serial.print(" - ");
  Serial.print(right_speed);
  Serial.println();
  wheels->commandSpeeds(left_speed, right_speed);
}
