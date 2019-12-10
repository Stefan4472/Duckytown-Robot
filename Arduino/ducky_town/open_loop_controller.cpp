#include "open_loop_controller.h"

void OpenLoopController::init()
{
  updatePeriodMs = 0;  // THIS NEEDS TO BE ZERO SO THAT OPENLOOP RUNS ON EACH UPDATE AND CAN USE ODOMETRY DIST/PREVDIST VALUES.
  lastUpdateMs = 0;  
}

void OpenLoopController::update(OdometryInterface* odometry, WheelInterface* wheels, LightsInterface* lights)
{
  unsigned long curr_time = millis();
  if (finished || curr_time - lastUpdateMs <= updatePeriodMs)
  {
    return;
  }
   
//  Serial.println("OpenLoop running update at time " + String(millis()));
//  Serial.println("currDist = " + String(odometry->distTravelled));
//  Serial.println("prevDist = " + String(odometry->prevDistTravelled));

  // Update tracked distance travelled.
  distanceTravelled += odometry->distTravelled - odometry->prevDistTravelled;
//  Serial.println("distanceTravelled now " + String(distanceTravelled));
  
  // Stop the motors and finish if the desired distance has been travelled
  if (abs(distanceTravelled) >= abs(targetDistance))
  {
//    Serial.println("Greater than targetDistance " + String(targetDistance) + " at time " + String(millis()));
    if (lastControl == OpenControlType::LEFT)
    {
      lights->stopLeftBlinker();
    }
    else if (lastControl == OpenControlType::RIGHT)
    {
      lights->stopRightBlinker();
    }
 
    wheels->commandPWMs(0, 0);
    finished = true;
  }

  lastUpdateMs = curr_time;
}

void OpenLoopController::commandStraight(float cmPerSec, float targetDistCm, WheelInterface* wheels)
{
//  Serial.println("Commanded straight " + String(targetDistCm) + " at speed " + String(cmPerSec));
  wheels->commandStraightSpeed(cmPerSec);
  
  distanceTravelled = 0.0;
  this->targetDistance = targetDistCm;
  
  lastControl = OpenControlType::STRAIGHT;
  finished = false;
}

void OpenLoopController::commandRightTurn(float cmPerSec, float turnRadius, float targetRad, WheelInterface* wheels, LightsInterface* lights)
{
//  Serial.println("Openloop received command to turn right");
  float left_speed = cmPerSec * (turnRadius + WHEEL_BASE_CM / 2.0) / turnRadius;
  float right_speed = cmPerSec * (turnRadius - WHEEL_BASE_CM / 2.0) / turnRadius;
  
  // Calculate desired straight-line distance (0.0 = no target).
  distanceTravelled = 0.0;
  targetDistance = targetRad * WHEEL_CIRCUMFERENCE_CM;
//  Serial.println("TargetDistance " + String(targetDistance) + ", speed = " + String(left_speed) + ", " + String(right_speed));
  lights->startRightBlinker();
  wheels->commandSpeeds(left_speed, right_speed);
  
  lastControl = OpenControlType::RIGHT;
  finished = false;
}

void OpenLoopController::commandLeftTurn(float cmPerSec, float turnRadius, float targetRad, WheelInterface* wheels, LightsInterface* lights)
{
//  Serial.println("Openloop received command to turn left");
  float left_speed = cmPerSec * (turnRadius - WHEEL_BASE_CM / 2.0) / turnRadius;
  float right_speed = cmPerSec * (turnRadius + WHEEL_BASE_CM / 2.0) / turnRadius;
  //  Serial.print("Calculated speeds " + String(left_speed) + ", " + String(right_speed));
  
  // Calculate desired straight-line distance (0.0 = no target).
  distanceTravelled = 0.0;
  targetDistance = targetRad * WHEEL_CIRCUMFERENCE_CM;
//  Serial.println("TargetDistance " + String(targetDistance) + ", speed = " + String(left_speed) + ", " + String(right_speed));
  
  lights->startLeftBlinker();
  wheels->commandSpeeds(left_speed, right_speed);
  
  lastControl = OpenControlType::LEFT;
  finished = false;
}

void OpenLoopController::cancel(LightsInterface* lights)
{
  if (!finished)
  {
    if (lastControl == OpenControlType::LEFT)
    {
      lights->stopLeftBlinker();
    }
    else if (lastControl == OpenControlType::RIGHT)
    {
      lights->stopRightBlinker();
    }
    
    finished = true; 
  }
}
