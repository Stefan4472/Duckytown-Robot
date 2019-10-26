#ifndef ICONTROLLER_H
#define ICONTROLLER_H

// Forward-declare required interfaces.
class OdometryInterface;
class WheelInterface;

/*
Controller interface.
*/
class IController
{
  public:
    // Commands odometry and wheels. Return false if the current command has finished executing.
    virtual void update(OdometryInterface* odometry, WheelInterface* wheels);
    float updatePeriodMs; // TODO: IMPLEMENT?
    bool finished;
};

#endif
