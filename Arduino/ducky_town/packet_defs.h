#ifndef PACKET_DEFS_H
#define PACKET_DEFS_H

struct PiToArduinoPacket 
{
  uint8_t commandID;
  float arg1;
  float arg2;
  float arg3;
  uint32_t seqNum;
};

const uint8_t PI_TO_ARDUINO_PACKET_LENGTH = 17;
const uint8_t ARDUINO_TO_PI_PACKET_LENGTH = 17;

enum class PiToArduinoCmd
{
  ECHO = 1,
  SET_SPEEDLIMIT = 2,
  SET_MOTORS = 3,
  GET_ODOMETRY = 5,
  GET_TICKS = 6,
  SET_OPENLOOP_STRAIGHT = 7,
  SET_OPENLOOP_R_CURVE = 8,
  SET_OPENLOOP_L_CURVE = 9,
  SET_CLOSEDLOOP = 10,
  RESET_ODOMETRY = 11,
  TURN_STATISTICS_ON = 12
};

enum class ArduinoToPiRsp
{
  ECHO = 1,
  UNRECOGNIZED_COMMAND = 2,
  INVALID_SYNTAX = 3,
  ILLEGIBLE_PACKET = 4,
  ODOMETRY = 5,
  TICKS = 6,
  CONTROL_FINISHED = 7,
  STATISTICS = 8
  //MOTOR_ERROR = 3
};

struct ArduinoToPiPacket 
{
  uint8_t commandID;
  float arg1;
  float arg2;
  float arg3;
  uint32_t seqNum;
};

#endif
