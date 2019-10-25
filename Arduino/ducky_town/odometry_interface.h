#ifndef ODOMETRY_INTERFACE_H
#define ODOMETRY_INTERFACE_H

class OdometryInterface
{   
  private: 
    // Values of leftCount and rightCount the last time 'getOdometry()'
    // was called.
    long prevLeftCount, prevRightCount;
    // 
    long prevX, prevY;
    float prevTheta;
    
  public:
    // Tracks the number of ticks on the left wheel.
    // Positive value = counter-clock-wise.
    volatile long leftCount; 
    // Tracks the number of ticks seen on the right wheel.
    // Positive value = counter-clock-wise
    volatile long rightCount; 

    // Initialize odometry. TODO: WHICH INTERRUPTS AND PINS DOES THIS SET?
    void init();
    // Get raw tick counts for each wheel.
    void getTickCounts(long* left, long* right);
    // Calculates current (x, y, theta) from the starting position.
    void getOdometry(long* x, long* y, float* theta);
    // Reset to some authoritative position.
    void resetTo(long x, long y, float theta);
};
#endif
