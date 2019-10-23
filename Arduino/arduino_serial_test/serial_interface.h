#ifndef SERIAL_INTERFACE_H
#define SERIAL_INTERFACE_H

#include "Arduino.h"
#include "packet_defs.h"

namespace SerialUtil 
{ 
  uint32_t readUint32();
  void writeUint32(uint32_t to_write);
    // TODO: ERROR CHECKING?
  bool readPacket(PiToArduinoPacket* packet);  // TODO: SUPPORT SIGNED VALUES
  bool writePacket(ArduinoToPiPacket* packet);
  void printPacket(PiToArduinoPacket* packet);
  void printPacket(ArduinoToPiPacket* packet);
}
#endif
