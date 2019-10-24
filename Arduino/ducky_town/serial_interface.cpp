#include "serial_interface.h"

int32_t SerialUtil::readInt32()
{
  int32_t read_int = 0;
  uint8_t next_byte;

  for (char i = 0; i < 4; i++) 
  {
    next_byte = Serial.read();
    read_int = (read_int << 8) | next_byte;
  }
  return read_int;
}

void SerialUtil::writeInt32(int32_t to_write)
{
  Serial.write((int8_t) ((to_write << 0) >> 24));
  Serial.write((int8_t) ((to_write << 8) >> 24));
  Serial.write((int8_t) ((to_write << 16) >> 24));
  Serial.write((int8_t) ((to_write << 24) >> 24));
}

uint32_t SerialUtil::readUint32()
{
  uint32_t read_int = 0;
  uint8_t next_byte;

  for (char i = 0; i < 4; i++) 
  {
    next_byte = Serial.read();
    read_int = (read_int << 8) | next_byte;
  }
  return read_int;
}

void SerialUtil::writeUint32(uint32_t to_write)
{
  Serial.write((uint8_t) ((to_write << 0) >> 24));
  Serial.write((uint8_t) ((to_write << 8) >> 24));
  Serial.write((uint8_t) ((to_write << 16) >> 24));
  Serial.write((uint8_t) ((to_write << 24) >> 24));
}

  // TODO: ERROR CHECKING
bool SerialUtil::readPacket(PiToArduinoPacket* packet) 
{
  packet->commandID = Serial.read();
  packet->arg1 = readInt32() / 1000.0;
  packet->arg2 = readInt32() / 1000.0;
  packet->arg3 = readInt32() / 1000.0;
  packet->seqNum = Serial.read();
  return true;
}

bool SerialUtil::writePacket(ArduinoToPiPacket* packet)
{
  Serial.write(packet->commandID);
  writeInt32((int32_t) (packet->arg1 * 1000.0));
  writeInt32((int32_t) (packet->arg2 * 1000.0));
  writeInt32((int32_t) (packet->arg3 * 1000.0));
  Serial.write(packet->seqNum);
  return true;
}

void SerialUtil::printPacket(PiToArduinoPacket* packet)
{
  Serial.print("PiToArduinoPacket: ");
  Serial.print(packet->commandID);
  Serial.print(", ");
  Serial.print(packet->arg1, 3);  // TODO: PRINT FLOATING POINT NUMBERS
  Serial.print(", ");
  Serial.print(packet->arg2, 3);
  Serial.print(", ");
  Serial.print(packet->arg3, 3);
  Serial.print(", ");
  Serial.print(packet->seqNum);
  Serial.println();
}

void SerialUtil::printPacket(ArduinoToPiPacket* packet)
{
  Serial.print("ArduinoToPiPacket: ");
  Serial.print(packet->commandID);
  Serial.print(", ");
  Serial.print(packet->arg1, 3);
  Serial.print(", ");
  Serial.print(packet->arg2, 3);
  Serial.print(", ");
  Serial.print(packet->arg3, 3);
  Serial.print(", ");
  Serial.print(packet->seqNum);
  Serial.println();
}
