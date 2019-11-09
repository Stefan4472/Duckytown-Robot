#ifndef PROXIMITY_SENSOR_H
#define PROXIMITY_SENSOR_H

// need to include any header files?

//use const float/int instead?
#define MICROSEC_PER_CM 29.15
#define PING_PIN 7
#define MAX_OBSTACLE_DIST 10

class ProximiySensor
(
  //private:
    
  public:
  
    float pingTime, obst_distance, microSecPerCM = MICROSEC_PER_CM;
    int pingPin = PING_PIN, maxObstacleDist = MAX_OBSTACLE_DIST;
    
    //Gets time for ping to return to robot
    float getPingTime(int pingPin);
    
    //Converts ping time to distance
    float getPingDist(float pingTime);
};

#endif
