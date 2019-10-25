#ifndef PACKET_DEFS_H
#define PACKET_DEFS_H

struct PiToArduinoPacket 
{
  uint8_t commandID;
  float arg1;
  float arg2;
  float arg3;
  uint8_t seqNum;
};

const uint8_t PI_TO_ARDUINO_PACKET_LENGTH = 14;
const uint8_t ARDUINO_TO_PI_PACKET_LENGTH = 14;

enum class PiToArduinoCmd
{
  ECHO = 1,
  SET_MOTORS = 3,
  GET_ODOMETRY = 5,
  GET_TICKS = 6
};

enum class ArduinoToPiRsp   // TODO: THREE LONGS
{
  ECHO = 1,
  UNRECOGNIZED_COMMAND = 2,
  INVALID_SYNTAX = 3,
  ILLEGIBLE_PACKET = 4,
  ODOMETRY = 5,
  TICKS = 6
  //MOTOR_ERROR = 3
};

struct ArduinoToPiPacket 
{
  uint8_t commandID;
  float arg1;
  float arg2;
  float arg3;
  uint8_t seqNum;
};

#endif
