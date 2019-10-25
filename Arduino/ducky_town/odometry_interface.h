#ifndef ODOMETRY_INTERFACE_H
#define ODOMETRY_INTERFACE_H

#include "Arduino.h"
//struct OdometryData
//{
//  long x, y;
//  float theta;
//  long leftCount, rightCount;
//  long distTravelled, distLeft, distRight;
//};

// Width between wheels (centimeters)
#define WHEEL_BASE_CM 17.5
// Radius of the wheel (centimeters)
#define WHEEL_RADIUS_CM 3.5
// Circumference of the wheel (centimeters)
const float WHEEL_CIRCUMFERENCE_CM = 2.0 * PI * WHEEL_RADIUS_CM;

class OdometryInterface
{   
  private: 
    // Values of leftCount and rightCount the last time 'getOdometry()'
    // was called.
    long prevLeftCount, prevRightCount;
    // 
    float prevX, prevY;
    float prevTheta;
    
  public:
    // Current position and orientation (from starting position) 
    // (x, y) are in centimeters, (theta) is in radians.
    float x, y;
    float theta;
    // TOTAL distance travelled
    long distTravelled;
    long distLeft, distRight;
    
    // Tracks the number of ticks on the left wheel.
    // Positive value = counter-clock-wise.
    volatile long leftCount; 
    // Tracks the number of ticks seen on the right wheel.
    // Positive value = counter-clock-wise
    volatile long rightCount; 

    // Initialize odometry. TODO: WHICH INTERRUPTS AND PINS DOES THIS SET?  TODO: TRACK DISTANCE_TRAVELLED ON EACH WEEL AND IN GENERAL. STRUCT ODOMETRY_INFORMATION? 
    void init();
    // Get raw tick counts for each wheel.
    void getTickCounts(long* left, long* right);
    // Updates odometry values, calculating (x, y, theta)
    void update();
    // Reset to some authoritative position.
    void resetTo(long x, long y, float theta);
};
#endif
