struct PiToArduinoPacket 
{
  uint8_t commandID;
  float arg1;
  float arg2;
  float arg3;
  uint8_t seqNum;
};

struct ArduinoToPiPacket 
{
  uint8_t commandID;
  float arg1;
  float arg2;
  float arg3;
  uint8_t seqNum;
};

namespace SerialUtil 
{ 
  uint32_t readUint32()
  {
    uint32_t read_int = 0;
    uint8_t next_byte;
  
    for (char i = 0; i < 4; i++) 
    {
      next_byte = Serial.read();
      read_int = (read_int << 8) | next_byte;
    }
//    Serial.print("Read int ");
//    Serial.print(read_int);
//    Serial.println();
    return read_int;
  }

  void writeUint32(uint32_t to_write)
  {
    Serial.write((uint8_t) ((to_write << 0) >> 24));
    Serial.write((uint8_t) ((to_write << 8) >> 24));
    Serial.write((uint8_t) ((to_write << 16) >> 24));
    Serial.write((uint8_t) ((to_write << 24) >> 24));
  }

    // TODO: ERROR CHECKING
  bool readPacket(PiToArduinoPacket* packet) 
  {
    packet->commandID = Serial.read();
    packet->arg1 = readUint32() / 1000.0;
    packet->arg2 = readUint32() / 1000.0;
    packet->arg3 = readUint32() / 1000.0;
    packet->seqNum = Serial.read();
    return true;
  }

  bool writePacket(ArduinoToPiPacket* packet)
  {
    Serial.write(packet->commandID);
    writeUint32((uint32_t) packet->arg1 * 1000.0);
    writeUint32((uint32_t) packet->arg2 * 1000.0);
    writeUint32((uint32_t) packet->arg3 * 1000.0);
    Serial.write(packet->seqNum);
    return true;
  }

  void printPacket(PiToArduinoPacket* packet)
  {
    Serial.print("PiToArduinoPacket: ");
    Serial.print(packet->commandID);
    Serial.print(", ");
    Serial.print(packet->arg1);  // TODO: PRINT FLOATING POINT NUMBERS
    Serial.print(", ");
    Serial.print(packet->arg2);
    Serial.print(", ");
    Serial.print(packet->arg3);
    Serial.print(", ");
    Serial.print(packet->seqNum);
    Serial.println();
  }

  void printPacket(ArduinoToPiPacket* packet)
  {
    Serial.print("ArduinoToPiPacket: ");
    Serial.print(packet->commandID);
    Serial.print(", ");
    Serial.print(packet->arg1);
    Serial.print(", ");
    Serial.print(packet->arg2);
    Serial.print(", ");
    Serial.print(packet->arg3);
    Serial.print(", ");
    Serial.print(packet->seqNum);
    Serial.println();
  }
}

// TODO: ECHO PACKET, ERROR PACKET, CURR_STATE PACKET
bool handleRequest(PiToArduinoPacket* request, ArduinoToPiPacket* response)
{
  response->commandID = request->commandID;
  response->arg1 = request->arg1;
  response->arg2 = request->arg2;
  response->arg3 = request->arg3;
  response->seqNum = request->seqNum;
  return true;
}

void setup() 
{
  Serial.begin(9600);  // TODO: RUN AT A HIGHER BAUD RATE
  Serial.flush();
}

void loop() 
{
//  if (Serial.available() >= 14)
//  {
//    while (Serial.available())
//    {
//      Serial.write(Serial.read());
//    }
////    Serial.println();
//    Serial.write('\n');
//  }
  

//  // Check if a packet has buffered  
  while (Serial.available() >= 14) 
  {
    Serial.print("Parsing serial data ");
    Serial.print(Serial.available());
    Serial.println(" bytes");
    PiToArduinoPacket recv_packet;
    bool got_packet = SerialUtil::readPacket(&recv_packet);

    if (got_packet) 
    {
      SerialUtil::printPacket(&recv_packet);
      // Create response
      ArduinoToPiPacket send_packet;
      if (handleRequest(&recv_packet, &send_packet))
        {
          SerialUtil::printPacket(&send_packet);
          SerialUtil::writePacket(&send_packet);
          Serial.write('\r');
          Serial.write('\n');
        }
    }
    else 
    {
      Serial.println("Couldn't read packet");
    }
  }
}
