#ifndef ICONTROLLER_H
#define ICONTROLLER_H

// Forward-declare required interfaces.
class OdometryInterface;
class WheelInterface;
class LightsInterface;

/*
Controller interface.
*/
class IController
{
  public:
    // Commands odometry, wheels, and lights. Return false if the current command has finished executing.
    virtual void update(OdometryInterface* odometry, WheelInterface* wheels, LightsInterface* lights);
    unsigned int updatePeriodMs;
    unsigned long lastUpdateMs;
    bool finished;
};

#endif
