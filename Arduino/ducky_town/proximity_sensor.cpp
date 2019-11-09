#include "proximity_sensor.h"
#include "Arduino.h"

//use const float/int instead?
#define MICROSEC_PER_CM 29.15

//use this line as condition for cutting wheel velocity
//if(getPingDist(getPingTime(pingPin)) < maxObstacleDist){

void ProximitySensor::init(int pin) 
{
  this->pin = pin;
}

float ProximitySensor::getDistCm()
{
  // TODO: ARE THESE STATEMENTS NECESSARY?
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
  delayMicroseconds(2);
  digitalWrite(pin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pin, LOW);
  pinMode(pin, INPUT);

  // Calculate distance in centimeters based on pulse time 
  return pulseIn(pin, HIGH) / MICROSEC_PER_CM / 2.0;
}

// TODO: MORE SOPHISTICATED
void ProximitySensor::runCollisionAvoidance(float dx, WheelInterface* wheels)
{
  float fwd_dist = getDistCm();

  if (fwd_dist < minFollowDistanceCm)
  {
    wheels->startSpeedOverride(0);
  }
  else if (!wheels->overrideOn)
  {
    wheels->stopSpeedOverride();
  }
}
