#include <math.h>
#include "DualMC33926MotorShield.h"
#include "packet_defs.h"
#include "serial_interface.h"

// MotorShield interface.
DualMC33926MotorShield motorInterface;

const int8_t TICKS_PER_ROTATION = 8; 
const int8_t lookup_table[] = {0,0,0,-1,0,0,1,0,0,1,0,0,-1,0,0,0};
const long WHEEL_BASE = 175; // width between wheels (in mm? smaller units are useful to avoid floats)
const long WHEEL_RADIUS = 35; // should be same unit as WHEEL_BASE
//const long CHASSIS_LENGTH = 120; // length from center of wheel axis to front point we are controlling 
const uint8_t SensorCount = 4;
uint16_t sensorValues[SensorCount];

volatile long left_count = 0; // track # of left wheel movements, CCW is positive
volatile long right_count = 0; // track # of right wheel movements
volatile long x_world = 0; // x pos in world frame (in mm? should be same as WHEEL_BASE)
volatile long y_world = 0; // y pos in world frame (in mm? should be same as WHEEL_BASE)
volatile long theta_world = 0; // angle pos in world frame (in rads?)

long prev_left_count = 0;
long prev_right_count = 0;

void setup() 
{
  Serial.begin(9600);  // TODO: RUN AT A HIGHER BAUD RATE
  Serial.flush();
  motorInterface.init();

  pinMode(2, INPUT);
  pinMode(3, INPUT);
  
  // setup interrupts
  attachInterrupt(digitalPinToInterrupt(2),left_wheel_isr,CHANGE);
  attachInterrupt(digitalPinToInterrupt(3),right_wheel_isr,CHANGE);
  
}

void loop() // TODO: A PROPER SERIALINTERFACE CLASS
{
  // Check if a packet has buffered  
  while (Serial.available() >= 14) 
  {
    PiToArduinoPacket recv_packet;
    bool got_packet = SerialUtil::readPacket(&recv_packet);

    if (got_packet) 
    {
      // Create response
      ArduinoToPiPacket send_packet;
      if (handleRequest(&recv_packet, &send_packet))
      {
          SerialUtil::writePacket(&send_packet);
      }
    }
    // Couldn't read a valid packet off the serial buffer.
    else 
    {
      // Create and write an error packet.
      ArduinoToPiPacket err_packet;
      err_packet.commandID = static_cast<int>(ArduinoToPiRsp::ILLEGIBLE_PACKET);
      SerialUtil::writePacket(&err_packet);
      // Flush any remaining serial input. TODO: NEED A BETTER RECOVERY PROCESS.
      Serial.flush();
    }
  }

  noInterrupts();
  long deltaRight = right_count - prev_right_count;
  long deltaLeft = left_count - prev_left_count;
  theta_world += delta_theta(deltaRight, deltaLeft);
  x_world += cos(theta_world) * delta_x_robot(deltaRight, deltaLeft); 
  y_world += sin(theta_world) * delta_x_robot(deltaRight, deltaLeft);
  prev_left_count = left_count;
  prev_right_count = right_count;
  interrupts();
  
  Serial.print(left_count);
  Serial.print('/');
  Serial.print(right_count);
  /*
  Serial.print('/');
  Serial.print(prev_left_count);
  Serial.print('/');
  Serial.print(prev_right_count);
  */
  Serial.println();
  
//  prev_right_count = right_count;
//  Serial.print(left_count);
//  Serial.println();
//  Serial.println(String(prev_left_count) + " " + String(left_count));
  delay(250);
  
}

// TODO: ECHO PACKET, ERROR PACKET (INCLUDING ERROR CODE), MOTOR_ERROR, CURR_STATE PACKET
// Returns whether to send the response.
bool handleRequest(PiToArduinoPacket* request, ArduinoToPiPacket* response)
{
  switch (static_cast<int>(request->commandID))
  {
    // ECHO command
    case static_cast<int>(PiToArduinoCmd::ECHO):
      response->commandID = static_cast<int>(ArduinoToPiRsp::ECHO);
      response->arg1 = request->arg1;
      response->arg2 = request->arg2;
      response->arg3 = request->arg3;
      response->seqNum = request->seqNum;
      return true;
      
    // SET_MOTORS command
    case static_cast<int>(PiToArduinoCmd::SET_MOTORS):
      motorInterface.setM1Speed((int) request->arg1);
      motorInterface.setM2Speed((int) request->arg2);
      return false;

    // GET_ODOMETRY command 
    case static_cast<int>(PiToArduinoCmd::GET_ODOMETRY):
      response->commandID = static_cast<int>(ArduinoToPiRsp::ODOMETRY);
      response->arg1 = x_world;
      response->arg2 = y_world;
      response->arg3 = theta_world;
      response->seqNum = request->seqNum;
      return true;

    // GET_TICKS command 
    case static_cast<int>(PiToArduinoCmd::GET_TICKS):
      response->commandID = static_cast<int>(ArduinoToPiRsp::TICKS);
      response->arg1 = (float) left_count;
      response->arg2 = (float) right_count;
      response->seqNum = request->seqNum;
      return true;

    // Unrecognized/unsupported commandID
    default: 
      response->commandID = static_cast<int>(ArduinoToPiRsp::UNRECOGNIZED_COMMAND);
      response->seqNum = request->seqNum;
      return true;
  }
}

// right wheel uses pins 3 and 11
void right_wheel_isr() {
    static uint8_t right_enc_val = 0;
    right_enc_val = right_enc_val << 2;
    
    uint8_t pin11 = (PINB & 0b1000) >> 3;
    uint8_t pin3 = (PIND & 0b1000) >> 2;
    
    right_enc_val = right_enc_val | (pin3 | pin11); // puts current values for pins 3 and 11 into enc_val
    right_count = right_count + lookup_table[right_enc_val & 0b1111];
    
}

// left wheel uses pins 2 and 5
void left_wheel_isr() {
    static uint8_t left_enc_val = 0;
    left_enc_val = left_enc_val << 2;

    uint8_t pin5 = (PIND & 0b100000) >> 5;
    uint8_t pin2 = (PIND & 0b100) >> 1;

    left_enc_val = left_enc_val | (pin2| pin5); // puts current values for pins 5 and 2 into enc_val
    left_count = left_count + lookup_table[left_enc_val & 0b1111];
}


long delta_theta(long deltaRight, long deltaLeft) {
  return atan2( (deltaRight - deltaLeft) * PI * WHEEL_RADIUS, WHEEL_BASE/2 );
}

// forward distance traveled in robot frame, use cos or sin to calculate in world frame
long delta_x_robot(long deltaRight, long deltaLeft) {
  return TICKS_PER_ROTATION * (deltaRight + deltaLeft) * PI * WHEEL_RADIUS;
}
