#include "closed_loop_controller.h"


//#define K_TRANS 4.0
//#define B_TRANS 4.5
//#define K_ROT 28.0
//#define B_ROT 2.0

#define K_TRANS 0.40
#define B_TRANS 0.80
#define K_ROT 8.0
#define B_ROT 4.5

// TEMPORARY
#define K 5.0
#define B 5.0

void ClosedLoopController::update(OdometryInterface* odometry, WheelInterface* wheels)
{
  /*
  Serial.println();
  Serial.println("Target " + String(targetX) + ", " + String(targetY) + ", " + String(targetTheta));
  // Calculate yoke point
  float yoke_x = odometry->x + CHASSIS_LENGTH_CM * cos(odometry->theta);
  float yoke_y = odometry->y + CHASSIS_LENGTH_CM * sin(odometry->theta);
  float yoke_t = odometry->theta;
  Serial.println("Yoke " + String(yoke_x) + ", " + String(yoke_y) + ", " + String(yoke_t));

  // Calculate yoke velocities
  float yoke_dx = odometry->dX + CHASSIS_LENGTH_CM * -sin(odometry->theta) * odometry->dTheta;
  float yoke_dy = odometry->dY + CHASSIS_LENGTH_CM * cos(odometry->theta) * odometry->dTheta;
  float yoke_dt = odometry->dTheta;
  Serial.println("DYoke " + String(yoke_dx) + ", " + String(yoke_dy) + ", " + String(yoke_dt));

  // Proportional error (position)
  float p_err_x = yoke_x - targetX;
  float p_err_y = yoke_y - targetY;
  Serial.println("pError " + String(p_err_x) + ", " + String(p_err_y));
  
  // Derivative error (velocity)
  float d_err_x = yoke_dx;
  float d_err_y = yoke_dy;
  Serial.println("dError " + String(d_err_x) + ", " + String(d_err_y));
  
  // Calculate total force, Fpd
  float fpd_x = -K * p_err_x - B * d_err_x;
  float fpd_y = -K * p_err_y - B * d_err_y;
  Serial.println("fPD " + String(fpd_x) + ", " + String(fpd_y));
  
  // Calculate translation force, Ftrans = <Fpd, yoke_dx>
  float ftrans = fpd_x * yoke_dx + fpd_y * yoke_dy;
  Serial.println("fTrans " + String(ftrans));
  
  // Calculate Moment of rotation, Mrot = a1b2 - a2b1
  float mrot_z = CHASSIS_LENGTH_CM * fpd_y;
  
  Serial.println("Left, right should be " + String(ftrans - mrot_z) + ", " + String(ftrans + mrot_z));
  wheels->commandPWMs(ftrans - mrot_z, ftrans + mrot_z);
  // Calculate elapsed time
  //  elapsedTime = millis() - prevTime;
  //  prevTime = millis();
  */
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
      wheels->commandPWMs(wheels->currLPWM + (trans_ddot - theta_ddot) * 0.97, wheels->currRPWM + trans_ddot + theta_ddot);
//    }
  }
}

void ClosedLoopController::doATurn(float radius, float velocity, bool left, long startTime)
{
  
}


void ClosedLoopController::commandPosition(float x, float y, float theta)
{
  Serial.println("Setting target " + String(x) + ", " + String(y) + ", " + String(theta));
  targetX = x;
  targetY = y;
  targetTheta = theta;
//  finished = false;
}
