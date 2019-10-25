#include "odometry_interface.h"
#include "Arduino.h"

const int8_t TICKS_PER_ROTATION = 8; 
const int8_t LOOKUP_TABLE[] = {0,0,0,-1,0,0,1,0,0,1,0,0,-1,0,0,0};
const long WHEEL_BASE_MM = 175; // width between wheels (in mm? smaller units are useful to avoid floats)
const long WHEEL_RADIUS_MM = 35; // should be same unit as WHEEL_BASE
//const long CHASSIS_LENGTH_MM = 120; // length from center of wheel axis to front point we are controlling 
const uint8_t NUM_SENSORS = 4;
uint16_t sensorValues[NUM_SENSORS];

// TODO: MAKE PART OF ODOMETRYINTERFACE CLASS
OdometryInterface* odometrySingleton;

// TODO: MOVE THESE STATIC DEFINITIONS BELOW THE CLASS DEFINITIONS
// ISR for the right wheel QTR sensors.
// Uses pins 3 and 11.
void right_wheel_isr() {
    static uint8_t right_enc_val = 0;
    right_enc_val = right_enc_val << 2;
    
    uint8_t pin11 = (PINB & 0b1000) >> 3;
    uint8_t pin3 = (PIND & 0b1000) >> 2;
    
    right_enc_val = right_enc_val | (pin3 | pin11); // puts current values for pins 3 and 11 into enc_val
    odometrySingleton->rightCount += LOOKUP_TABLE[right_enc_val & 0b1111];
}

// ISR for the left wheel QTR sensors.
// Uses pins 2 and 5.
void left_wheel_isr() {
    static uint8_t left_enc_val = 0;
    left_enc_val = left_enc_val << 2;

    uint8_t pin5 = (PIND & 0b100000) >> 5;
    uint8_t pin2 = (PIND & 0b100) >> 1;

    left_enc_val = left_enc_val | (pin2| pin5); // puts current values for pins 5 and 2 into enc_val
    odometrySingleton->leftCount += LOOKUP_TABLE[left_enc_val & 0b1111];
}

// TODO: MAKE INTO A MACRO
long delta_theta(long deltaRight, long deltaLeft) {
  return atan2( (deltaRight - deltaLeft) * PI * WHEEL_RADIUS_MM, WHEEL_BASE_MM/2 );
}

// forward distance traveled in robot frame, use cos or sin to calculate in world frame
long delta_x_robot(long deltaRight, long deltaLeft) {
  return TICKS_PER_ROTATION * (deltaRight + deltaLeft) * PI * WHEEL_RADIUS_MM;
}

void OdometryInterface::init()
{
  pinMode(2, INPUT);
  pinMode(3, INPUT);

  // Set the singleton to this instance.
  odometrySingleton = this;
  
  // Setup interrupts.
  attachInterrupt(digitalPinToInterrupt(2), left_wheel_isr, CHANGE);
  attachInterrupt(digitalPinToInterrupt(3), right_wheel_isr, CHANGE);
}

void OdometryInterface::getTickCounts(long* left, long* right)
{
  noInterrupts();  // TODO: IS THIS NECESSARY?
  *left = this->leftCount;
  *right = this->rightCount;
  interrupts();
}

void OdometryInterface::getOdometry(long* x, long* y, float* theta)
{
  /*Serial.print(right_count);
  Serial.print('/');
  Serial.print(left_count);
  Serial.println("Updating odometry");*/
  long left, right;
  long d_left, d_right;
  long dist_left, dist_right;
  long dist_travelled;
  float heading_change;

  // Grap values for left and right TODO: MAKE VOLATILE?
  noInterrupts();  // TODO: IS THIS NECESSARY?
  right = this->rightCount;
  left = this->leftCount;
  interrupts();

  // Calculate changes
  d_right = right - this->prevRightCount;
  d_left = left - this->prevLeftCount;

  // Calculate distance travelled by each wheel 
  dist_left = (d_left / TICKS_PER_ROTATION) * 2.0 * PI * WHEEL_RADIUS_MM;
  dist_right = (d_right / TICKS_PER_ROTATION) * 2.0 * PI * WHEEL_RADIUS_MM;

  // Calculate distance travelled along current heading 
  dist_travelled = (dist_left + dist_right) / 2.0;

  // Calculate heading change
  heading_change = atan2((dist_right - dist_left) / 2.0, WHEEL_BASE_MM / 2.0);
  
  // Calculate new position
  this->prevTheta += heading_change;
  this->prevX += dist_travelled * cos(this->prevTheta); 
  this->prevY += dist_travelled * sin(this->prevTheta);
  
  this->prevLeftCount = left;
  this->prevRightCount = right;

  // Set values.
  *x = this->prevX;
  *y = this->prevY;
  *theta = this->prevTheta;
}

void OdometryInterface::resetTo(long x, long y, float theta)
{
  noInterrupts(); // TODO: IS THIS NECESSARY?
  this->rightCount = 0;
  this->leftCount = 0;
  interrupts();
  
  this->prevX = x;
  this->prevY = y;
  this->prevTheta = theta;
}


 
