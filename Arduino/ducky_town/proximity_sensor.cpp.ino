#include "proximity_sensor.h"

//use this line as condition for cutting wheel velocity
//if(getPingDist(getPingTime(pingPin)) < maxObstacleDist){

float Proximity_Sensor::getPingTime(int pingPin){
      //shouldn't need the serial.begin b/c we won't need readouts?
      //Serial.begin(9600);

      pinMode(pingPin, OUTPUT);
      digitalWrite(pingPin, LOW);
      delayMicroseconds(2);
      digitalWrite(pingPin, HIGH);
      delayMicroseconds(5);
      digitalWrite(pingPin, LOW);
      pinMode(pingPin, INPUT);
      return pingTime = pulseIn(pingPin, HIGH);
    }

float Proximity_Sensor::getPingDist(float pingTime){
      return obst_distance = (pingTime/microSecPerCM)/2;
    }



