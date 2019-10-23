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

enum class PiToArduinoCmd
{
  ECHO = 1,
  SET_MOTORS = 3,
};

enum class ArduinoToPiRsp
{
  ECHO = 1,
  UNRECOGNIZED_COMMAND = 2,
  INVALID_SYNTAX = 3,
  ILLEGIBLE_PACKET = 4
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
