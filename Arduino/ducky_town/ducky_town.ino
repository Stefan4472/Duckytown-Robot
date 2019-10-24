#include <math.h>
#include "packet_defs.h"
#include "serial_interface.h"
#include "odometry_interface.h"
#include "wheel_interface.h"

WheelInterface wheelInterface;

//OdometryInterface odometryInterface;

void setup() 
{
  Serial.begin(9600);  // TODO: RUN AT A HIGHER BAUD RATE
  Serial.flush();
  OdometryInterface::init();
  wheelInterface.init();
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

  OdometryInterface::update();
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
      Serial.println("Commanding PWMs");
      Serial.print((int) request->arg1);
      Serial.print('/');
      Serial.print((int) request->arg2);
      Serial.println();
      wheelInterface.commandPWMs((int) request->arg1, (int) request->arg2);
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
