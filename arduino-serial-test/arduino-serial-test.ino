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
  
};

void setup() 
{
  Serial.begin(9600);  // TODO: RUN AT A HIGHER BAUD RATE
  Serial.flush();
}

void loop() 
{
  // Check if a packet has buffered.
  while (Serial.available() >= 14) 
  {
    PiToArduinoPacket packet;
    bool got_packet = readPacket(&packet);

    if (got_packet) 
    {
      
    }
    
  }
}

// TODO: ERROR CHECKING
bool readPacket(PiToArduinoPacket* packet) 
{
//  uint8_t command_id = 
  packet->commandID = Serial.read();
  packet->arg1 = readUint32() * 1000.0;
  packet->arg2 = readUint32() * 1000.0;
  packet->arg3 = readUint32() * 1000.0;
  packet->seqNum = Serial.read();
}

uint16_t readUint32()
{
  uint32_t u_int = 0;
  uint8_t next_byte;

  for (char i = 0; i < 4; i++) 
  {
    next_byte = Serial.read();
    u_int = (u_int << 8) | next_byte;
  }
}
