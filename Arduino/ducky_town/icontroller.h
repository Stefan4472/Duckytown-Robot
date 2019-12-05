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
  protected:
    unsigned int updatePeriodMs;
    unsigned long lastUpdateMs;
    
  public:
    // Commands odometry, wheels, and lights. Return false if the current command has finished executing.
    virtual void update(OdometryInterface* odometry, WheelInterface* wheels, LightsInterface* lights);
    // Breaks the last command, if exists
    virtual void cancel(LightsInterface* lights);
    // Whether the last controlled thing has been finished
    bool finished;
};

#endif
