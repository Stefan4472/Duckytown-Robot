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
  // Send the pulse
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
  delayMicroseconds(2);
  digitalWrite(pin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pin, LOW);
  pinMode(pin, INPUT);

  float pulse_time = (float) pulseIn(pin, HIGH);
//  Serial.println(String(pulse_time) + ", " + String(pulse_time / MICROSEC_PER_CM / 2.0));
  
  // Calculate distance in centimeters based on pulse time 
  return pulse_time / MICROSEC_PER_CM / 2.0;
}

// TODO: MORE SOPHISTICATED
void ProximitySensor::runCollisionAvoidance(WheelInterface* wheels, float dX)
{
  float fwd_dist = getDistCm();

  // Turn braking override on if within minimum following distance.
  if (fwd_dist < minFollowDistanceCm)
  {
    wheels->startSpeedOverride(0);
//    Serial.println("Overriding");
  }
  // Turn braking override off if it is on unneccessarily. 
  if (wheels->overrideOn && fwd_dist >= minFollowDistanceCm)
  {
    wheels->stopSpeedOverride();
  }
}
