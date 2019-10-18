#include <QTRSensors.h>
#include <math.h> 

QTRSensors qtr;

const int8_t lookup_table[] = {0,0,0,-1,0,0,1,0,0,1,0,0,-1,0,0,0};
const float WHEEL_BASE =  17.8; // width between wheels (in cm? floats have 6-7 digits total precision)
const float WHEEL_RADIUS = 12; // should be same unit as WHEEL_BASE
//const float CHASSIS_LENGTH = 26; // length from center of wheel axis to front point we are controlling 
const uint8_t SensorCount = 4;
uint16_t sensorValues[SensorCount];

volatile long prev_left_count = 0;
volatile long left_count = 0; // track # of left wheel movements, CCW is positive
volatile long prev_right_count = 0;
volatile long right_count = 0; // track # of right wheel movements
volatile long x_world = 0; // x pos in world frame (in cm? should be same as WHEEL_BASE)
volatile long y_world = 0; // y pos in world frame (in cm? should be same as WHEEL_BASE)
volatile long theta_world = 0; // angle pos in world frame (in rads?)


void setup() {
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  
  // configure the sensors
  qtr.setTypeRC();
  qtr.setSensorPins((const uint8_t[]){2,3,4,5}, SensorCount);
  
  Serial.begin(9600);
  
  // setup interrupts
  attachInterrupt(digitalPinToInterrupt(2),left_wheel_isr,CHANGE);
  attachInterrupt(digitalPinToInterrupt(3),right_wheel_isr,CHANGE);
}

// takes reading from qtr sensor, returns 1 if reading black, 0 if white
uint8_t black_or_white(int sensorValue) {
  if (sensorValue < 2300) {
    return 0b0;
  } else {
    return 0b1;
  }
}

// right wheel uses pins 3 and 4
void right_wheel_isr() {
    volatile static uint8_t right_enc_val = 0;
    
    right_enc_val = right_enc_val << 2;

    uint8_t pin4 = black_or_white(sensorValues[2]);
    uint8_t pin3 = black_or_white(sensorValues[1]);
    right_enc_val = right_enc_val | ((pin4 << 1) | pin3); // puts current values fo pins 5 and 2 into enc_val
    right_count = right_count + lookup_table[right_enc_val & 0b1111];
}

// left wheel uses pins 2 and 5
void left_wheel_isr() {
    volatile static uint8_t left_enc_val = 0;
    
    left_enc_val = left_enc_val << 2;

    uint8_t pin5 = black_or_white(sensorValues[3]);
    uint8_t pin2 = black_or_white(sensorValues[0]);
    left_enc_val = left_enc_val | ((pin5 << 1) | pin2); // puts current values fo pins 5 and 2 into enc_val
    left_count = left_count + lookup_table[left_enc_val & 0b1111];
}


long delta_theta(long deltaRight, long deltaLeft) {
  return atan2( (deltaRight - deltaLeft) * PI * WHEEL_RADIUS, WHEEL_BASE/2 );
}

// forward distance traveled in robot frame, use cos or sin to calculate in world frame
long delta_x_robot(long deltaRight, long deltaLeft) {
  return (deltaRight + deltaLeft) * PI * WHEEL_RADIUS;
}


void loop() {
  prev_left_count = left_count;
  prev_right_count = right_count;
  qtr.read(sensorValues);

//  Serial.print(left_count);
//  Serial.println();
//  Serial.println(String(prev_left_count) + " " + String(left_count));

  long deltaRight = right_count - prev_right_count;
  long deltaLeft = left_count - prev_left_count;
  theta_world += delta_theta(deltaRight, deltaLeft);
  x_world += cos(theta_world) * delta_x_robot(deltaRight, deltaLeft); // some error since theta is not constant
  y_world += sin(theta_world) * delta_x_robot(deltaRight, deltaLeft);

//  Serial.println(String(prev_left_count) + " " + String(left_count));
  delay(250);
}
