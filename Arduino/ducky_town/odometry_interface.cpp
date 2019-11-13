#include "odometry_interface.h"
#include "Arduino.h"

// Number of ticks on the wheel.
#define TICKS_PER_ROTATION 8.0

const uint8_t NUM_SENSORS = 4;
uint16_t sensorValues[NUM_SENSORS];
const int8_t LOOKUP_TABLE[] = {0,0,0,-1,0,0,1,0,0,1,0,0,-1,0,0,0};

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
    odometrySingleton->rightCount -= LOOKUP_TABLE[right_enc_val & 0b1111];
}

// ISR for the left wheel QTR sensors.
// Uses pins 2 and 5.
void left_wheel_isr() {
    static uint8_t left_enc_val = 0;
    left_enc_val = left_enc_val << 2;

    uint8_t pin5 = (PIND & 0b100000) >> 5;
    uint8_t pin2 = (PIND & 0b100) >> 1;

    left_enc_val = left_enc_val | (pin2| pin5); // puts current values for pins 5 and 2 into enc_val
    odometrySingleton->leftCount -= LOOKUP_TABLE[left_enc_val & 0b1111];
}

void OdometryInterface::init()
{
  pinMode(2, INPUT);
  pinMode(3, INPUT);

  // Set the singleton to this instance.
  odometrySingleton = this;

  lastUpdateMs = millis();
  lastVelUpdateMs = lastUpdateMs;
  
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

void OdometryInterface::update()
{
//  Serial.println("Updating odometry");
  /*Serial.print(right_count);
  Serial.print('/');
  Serial.print(left_count);
  Serial.println("Updating odometry");*/
  // Variables prefixed with 'd' indicate a difference/derivative.
  long ticks_left, ticks_right;
  long dticks_left, dticks_right;
  float ddist_left, ddist_right;
  float ddist_travelled;
  float dtheta;

  // Grap values for left and right TODO: MAKE VOLATILE?
  noInterrupts();  // TODO: IS THIS NECESSARY?
  ticks_left = this->leftCount;
  ticks_right = this->rightCount;
  interrupts();
//  Serial.println("Ticks left " + String(ticks_left) + ", " + String(ticks_right));
  // Calculate changes
  dticks_left = ticks_left - this->prevLeftCount;
  dticks_right = ticks_right - this->prevRightCount;

  // Calculate distance travelled by each wheel
  ddist_left = (dticks_left / TICKS_PER_ROTATION) * WHEEL_CIRCUMFERENCE_CM;
  ddist_right = (dticks_right / TICKS_PER_ROTATION) * WHEEL_CIRCUMFERENCE_CM;

  // Calculate distance travelled along current heading
  ddist_travelled = (ddist_left + ddist_right) / 2.0;

  // Calculate heading change
  dtheta = atan2((ddist_right - ddist_left) / 2.0, WHEEL_BASE_CM / 2.0);

  // Calculate new position
  this->theta = this->prevTheta + dtheta;
  this->x = this->prevX + ddist_travelled * cos(this->theta);
  this->y = this->prevY + ddist_travelled * sin(this->theta);

  unsigned long curr_time_ms = millis();

  // Update velocity estimates every 500 ms
  if (curr_time_ms - lastVelUpdateMs >= 500)
  {
//    Serial.println("Curr time " + String(curr_time_ms) + " last update " + String(lastVelUpdateMs));
    //Serial.println("x, prev x " + String(x) + ", " + String(prevX));
//    Serial.println("Seconds since last update " + String((curr_time_ms - lastVelUpdateMs) / 1000.0));
    this->dX = (this->x - this->xLastVelUpdate) / ((curr_time_ms - lastVelUpdateMs) / 1000.0);
    this->dY = (this->y - this->yLastVelUpdate) / ((curr_time_ms - lastVelUpdateMs) / 1000.0);
    this->dTheta = (this->theta - this->tLastVelUpdate) / ((curr_time_ms - lastVelUpdateMs) / 1000.0);
//    Serial.println("dx " + String(dX) + " dy " + String(dY) + " dt " + String(dTheta));

    xLastVelUpdate = x;
    yLastVelUpdate = y;
    tLastVelUpdate = theta;
    lastVelUpdateMs = curr_time_ms;
  }
  
  

  this->prevLeftCount = ticks_left;
  this->prevRightCount = ticks_right;

  this->prevDistTravelled = this->distTravelled;
  this->distTravelled += ddist_travelled;

  // this->distLeft += ddist_left;
  // this->distRight += ddist_right;

  // Set values.
  this->prevX = this->x;
  this->prevY = this->y;
  this->prevTheta = this->theta;
  lastUpdateMs = curr_time_ms;
}

void OdometryInterface::resetTo(float x, float y, float theta)
{
  Serial.println("Resetting odometry");
  noInterrupts(); // TODO: IS THIS NECESSARY?
  this->rightCount = 0;
  this->leftCount = 0;
  interrupts();

  this->x = x;
  this->y = y;
  this->theta = theta;
  this->distTravelled = 0;

  this->prevX = x;
  this->prevY = y;
  this->prevTheta = theta;
  this->prevDistTravelled = 0;
//  this->distTravelled = 0;  // TODO: HOW TO GET THESE VALUES WITHOUT RESETTING?
//  this->distLeft = 0;
//  this->distRight = 0;
}
