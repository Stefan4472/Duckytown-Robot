#include "proximity_sensor.h"
#include "Arduino.h"
//use const float/int instead?
#define MICROSEC_PER_CM 29.15
#define MAX_OBSTACLE_DIST 10

//use this line as condition for cutting wheel velocity
//if(getPingDist(getPingTime(pingPin)) < maxObstacleDist){

void ProximitySensor::init(int pin) 
{
  this->pin = pin;
  pinMode(pin, OUTPUT);
}

float ProximitySensor::getDistCm()
{
  digitalWrite(pin, LOW);
  delayMicroseconds(2);
  digitalWrite(pin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pin, LOW);
  pinMode(pin, INPUT);

  double pingTime = pulseIn(pin, HIGH);
  return pingTime / MICROSEC_PER_CM / 2.0;
}
