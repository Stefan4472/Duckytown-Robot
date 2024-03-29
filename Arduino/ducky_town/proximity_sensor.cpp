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

void ProximitySensor::runCollisionAvoidance(float dx, WheelInterface* wheels)
{
  float fwd_dist = getDistCm();
  
  // Turn on override.
  if (fwd_dist < minFollowDistanceCm)
  {
    wheels->startSpeedOverride(0.0);
    //wheels->commandPWMs(0, 0);
//    Serial.println("Overriding (dist " + String(fwd_dist) + ")");
  }
  // Turn off override.
  if (fwd_dist >= minFollowDistanceCm && wheels->overrideOn)
  {
    //Serial.println("Override off");
    wheels->stopSpeedOverride();
  }
  
}
