#ifndef ODOMETRY_INTERFACE_H
#define ODOMETRY_INTERFACE_H

// TODO: PUT THESE IN A CLASS OR A NAMESPACE OR SOMETHING
extern volatile long left_count; // track # of left wheel movements, CCW is positive
extern volatile long right_count; // track # of right wheel movements
extern volatile long x_world; // x pos in world frame (in mm? should be same as WHEEL_BASE)
extern volatile long y_world; // y pos in world frame (in mm? should be same as WHEEL_BASE)
extern volatile long theta_world; // angle pos in world frame (in rads?)

extern long prev_left_count;
extern long prev_right_count;

namespace OdometryInterface
{
  void init();
  void update();
};
#endif
