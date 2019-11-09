const int pingPin=7, maxObstacleDist=10;
const float microSecPerCM=29.15;
float pingTime, obst_distance;

//use this line as condition for cutting wheel velocity
//if(getPingDist(getPingTime(pingPin)) < maxObstacleDist){

float getPingTime(int pingPin){
      //Serial.begin(9600); >> shouldn't need
      pinMode(pingPin, OUTPUT);
      digitalWrite(pingPin, LOW);
      delayMicroseconds(2);
      digitalWrite(pingPin, HIGH);
      delayMicroseconds(5);
      digitalWrite(pingPin, LOW);
      pinMode(pingPin, INPUT);
      return pingTime = pulseIn(pingPin, HIGH);
    }

float getPingDist(float pingTime){
      return obst_distance = (pingTime/microSecPerCM)/2;
    }
