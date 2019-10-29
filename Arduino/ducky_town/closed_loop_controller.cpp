#include "closed_loop_controller.h"

void ClosedLoopController::update(OdometryInterface* odometry, WheelInterface* wheels)
{
  // Calculate yoke point
  float yoke_x = odometry->x + CHASSIS_LENGTH_CM * cos(odometry->theta);
  float yoke_y = odometry->y + CHASSIS_LENGTH_CM * sin(odometry->theta);
  float yoke_t = odometry->theta;

  // Calculate yoke velocities
  float yoke_dx = odometry->dX + CHASSIS_LENGTH_CM * -sin(odometry->theta) * odometry->dTheta;
  float yoke_dy = odometry->dY + CHASSIS_LENGTH_CM * cos(odometry->theta) * odometry->dTheta;
  float yoke_dt = odometry->dTheta;

  // Proportional error (position)
  float p_err_x = yoke_x - targetX;
  float p_err_y = yoke_y - targetY;

  // Derivative error (velocity)
  float d_err_x = yoke_dx - 0;
  float d_err_y = yoke_dy - 0;

  // Calculate total force, Fpd
  float fpd_x = -K * p_err_x - B * d_err_x;
  float fpd_y = -K * p_err_y - B * d_err_y;

  // Calculate translation force, Ftrans = <Fpd, yoke_dx>
  float ftrans = fpd_x * yoke_dx + fpd_y * yoke_dy;

  // Calculate Moment of rotation, Mrot = a1b2 - a2b1
  float mrot_z = CHASSIS_LENGTH_CM * fpd_y;

  wheels->commandPWMs(ftrans - mrot_z, ftrans + mrot_z);
}

void ClosedLoopController::commandPosition(float x, float y, float theta)
{
  targetX = x;
  targetY = y;
  targetTheta = theta;
}
