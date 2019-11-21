#ifndef SERIAL_INTERFACE_H
#define SERIAL_INTERFACE_H

#include "Arduino.h"
#include "packet_defs.h"

// USES PINS 0 AND 1
class SerialInterface
{
  public:
    void init(long baudrate);
    // Gets the next packet of the serial interface, if one is 
    // there. Returns whether a packet was read.
    bool getNextPacket(PiToArduinoPacket* packet);
    // Send packet over serial.
    void sendPacket(ArduinoToPiPacket* packet);
};

// TODO: MOVE THIS OUT TO A DIFFERENT FILE
namespace SerialUtil 
{ 
  int32_t readInt32();
  void writeInt32(int32_t to_write);
  uint32_t readUint32();
  void writeUint32(uint32_t to_write);
    // TODO: ERROR CHECKING?
  bool readPacket(PiToArduinoPacket* packet);  // TODO: SUPPORT SIGNED VALUES
  bool writePacket(ArduinoToPiPacket* packet);
  void printPacket(PiToArduinoPacket* packet);
  void printPacket(ArduinoToPiPacket* packet);
}
#endif
