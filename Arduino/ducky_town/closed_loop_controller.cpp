#include "closed_loop_controller.h"


//#define K_TRANS 4.0
//#define B_TRANS 4.5
//#define K_ROT 28.0
//#define B_ROT 2.0

#define K_TRANS 0.05
#define B_TRANS 0.1
#define K_ROT 0.4 // 1.6
#define B_ROT 0.2 // 1.8

void ClosedLoopController::update(OdometryInterface* odometry, WheelInterface* wheels)
{
  /********************************************Theta-only*******************************************/
  bool simpleControl = true;
  if(simpleControl)
  {
    float theta_error = -targetTheta; // may be flipped
    float theta_dot_error = odometry->dTheta;
    float theta_ddot = -K_ROT * theta_error - B_ROT * theta_dot_error;
    
    wheels->commandSpeeds(this->speedLim - theta_ddot, this->speedLim + theta_ddot);
  
    return;
  }
  /**************************************************************************************************/
  
  if (!finished)
  { 
    float yoke_x = odometry->x + CHASSIS_LENGTH_CM * cos(odometry->theta);
    float yoke_y = odometry->y + CHASSIS_LENGTH_CM * sin(odometry->theta);
    float yoke_t = odometry->theta;
    
    float x_error = yoke_x - targetX;
    float y_error = yoke_y - targetY;
    float theta_error = yoke_t - targetTheta;
    float theta_dot_error = odometry->dTheta;
    
    float trans_error = x_error * cos(odometry->theta) + y_error * sin(odometry->theta);
    float trans_vel = odometry->dX *cos(odometry->theta) + odometry->dY * sin(odometry->theta);
   
    while(theta_error > M_PI) theta_error -= 2.0*M_PI;
    while(theta_error < -M_PI) theta_error += 2.0*M_PI;
   
    float theta_ddot = -K_ROT * theta_error - B_ROT * theta_dot_error;
    float trans_ddot = -K_TRANS*trans_error - B_TRANS*trans_vel;

    //    Serial.print("X: " + String(odometry->x));
//    Serial.print(" | Y: " + String(odometry->y));
//    Serial.println(" | T: " + String(yoke_t));
////    
    Serial.print("errX: " + String(x_error));
    Serial.print(" | errY: " + String(y_error));
    Serial.println(" | errT: " + String(theta_error));
//
//    if (sqrt(x_error*x_error + y_error*y_error) < 4.0)
//    {
//      if (onConvergedFcn)
//      {
//        onConvergedFcn();
//      }
//
//      finished = true; 
//      wheels->commandPWMs(0, 0);
//    }
//    else 
//    {
      finished = false;

      float curr_left_vel = wheels->leftSpeedFromPWM(wheels->currLeftPWM);
      float curr_right_vel = wheels->rightSpeedFromPWM(wheels->currRightPWM);
      wheels->commandSpeeds(curr_left_vel + trans_ddot - theta_ddot, curr_right_vel + trans_ddot + theta_ddot);
//    }
  }
}

void ClosedLoopController::setThetaControl(float speed_lim, float theta_rel)
{
	this->targetTheta = theta_rel;
	this->speedLim = speed_lim;
}


void ClosedLoopController::commandPosition(float x, float y, float theta)
{
  Serial.println("Setting target " + String(x) + ", " + String(y) + ", " + String(theta));
  targetX = x;
  targetY = y;
  targetTheta = theta;
//  finished = false;
}
