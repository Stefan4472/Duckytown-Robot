
#include "DualMC33926MotorShield.h"
#include "packet_defs.h"
#include "serial_interface.h"

// MotorShield interface.
DualMC33926MotorShield motorInterface;

void setup() 
{
  Serial.begin(9600);  // TODO: RUN AT A HIGHER BAUD RATE
  Serial.flush();
  motorInterface.init();
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
      
    // Unrecognized/unsupported commandID
    default: 
      response->commandID = static_cast<int>(ArduinoToPiRsp::UNRECOGNIZED_COMMAND);
      response->seqNum = request->seqNum;
      return true;
  }
}
