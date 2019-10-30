#include "closed_loop_controller.h"


#define K_TRANS 3.5
#define B_TRANS 5.0
#define K_ROT 1.0
#define B_ROT 1.0

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
  float p_err_x = targetX - yoke_x;
  float p_err_y = targetY - yoke_y;
  Serial.println("pError " + String(p_err_x) + ", " + String(p_err_y));
  
  // Derivative error (velocity)
  float d_err_x = 0-yoke_dx;
  float d_err_y = 0-yoke_dy;
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
  wheels->commandPWMs(ftrans - mrot_z, ftrans + mrot_z);*/
  // Calculate elapsed time
  //  elapsedTime = millis() - prevTime;
  //  prevTime = millis();
 
  bool PD_CONTROL = true;
  if (PD_CONTROL)
  {
    float error[2];
    float theta_ref, x_ref, y_ref;
    float x_dot_act, y_dot_act, theta_dot_act;
    float theta_error, theta_dot_error;
    float trans_error, trans_vel;
    float theta_ddot, trans_ddot;
    float yoke_x = odometry->x + CHASSIS_LENGTH_CM * cos(odometry->theta);
    float yoke_y = odometry->y + CHASSIS_LENGTH_CM * sin(odometry->theta);
    float yoke_t = odometry->theta;
    int X = 0, Y = 1;
    x_ref = targetX;
    y_ref = targetY;
    theta_ref = targetTheta;
   
    x_dot_act = odometry->dX;
    y_dot_act = odometry->dY;
    theta_dot_act = odometry->dTheta;
 
    error[X] = targetX - yoke_x;
    error[Y] = targetY - yoke_y;
    error[X] = -error[X];
    error[Y] = -error[Y];

    trans_error = error[X]*cos(odometry->theta) + error[Y]*sin(odometry->theta);
    trans_vel = x_dot_act*cos(odometry->theta) + y_dot_act*sin(odometry->theta);
    theta_error = theta_ref-odometry->theta;
    theta_dot_error = 0.0 - theta_dot_act;
    while(theta_error > M_PI) theta_error -= 2.0*M_PI;
    while(theta_error < -M_PI) theta_error += 2.0*M_PI;

    Serial.print("errX: " + String(error[X]));
    Serial.print(" | errY: " + String(error[Y]));
    Serial.println(" | errT: " + String(theta_error));

    Serial.print("X: " + String(odometry->x));
    Serial.print(" | Y: " + String(odometry->y));
    Serial.println(" | T: " + String(yoke_t));
   
    theta_ddot = -K_ROT * theta_error - B_ROT * theta_dot_error;
    trans_ddot = -K_TRANS*trans_error - B_TRANS*trans_vel;
 
    bool transTau = true;
    float m1new = 0, m2new = 0;
    float m1old, m2old;
    if(transTau)
    {
//      m1new = baseJT[0][0]*theta_ddot + baseJT[0][1]*trans_ddot;
//      m2new = baseJT[1][0]*theta_ddot + baseJT[1][1]*trans_ddot;
        m1new = trans_ddot - theta_ddot;
        m2new = trans_ddot + theta_ddot;
    } else {
//      m1new = m1old + trans_ddot - theta_ddot;
//      m2new = m2old + trans_ddot + theta_ddot;
    }

    if (abs(error[X]) + abs(error[Y]) < 3.0)
    {
      finished = true;
      return;
    }
    
    wheels->commandPWMs(m1new, m2new);
//    motorInterface.setM1Speed(m1new);
//    motorInterface.setM2Speed(m2new);      
  }
}

void ClosedLoopController::commandPosition(float x, float y, float theta)
{
  targetX = x;
  targetY = y;
  targetTheta = theta;
}
