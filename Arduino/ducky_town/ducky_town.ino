#include <math.h>
#include "packet_defs.h"
#include "serial_interface.h"
#include "odometry_interface.h"
#include "wheel_interface.h"

SerialInterface serialInterface;
WheelInterface wheelInterface;
OdometryInterface odometryInterface;

// TODO: RUN AT A HIGHER BAUD RATE
const long BAUD_RATE = 9600;

void setup() 
{
  serialInterface.init(BAUD_RATE);
  odometryInterface.init();
  wheelInterface.init();
}

void loop()
{
  static PiToArduinoPacket recv_packet;
  static ArduinoToPiPacket send_packet;
  
  // Process any queued packets.
  while (serialInterface.getNextPacket(&recv_packet))
  {
    // Create response
    ArduinoToPiPacket send_packet;
    if (handleRequest(&recv_packet, &send_packet))
    {
      serialInterface.sendPacket(&send_packet);
    }
  }

  long left, right;
  odometryInterface.getTickCounts(&left, &right);
  Serial.print(left);
  Serial.print(' ');
  Serial.print(right);
  Serial.println();
  long x, y;
  float theta;
  odometryInterface.getOdometry(&x, &y, &theta);
  Serial.print(x);
  Serial.print(' ');
  Serial.print(y);
  Serial.print(' ');
  Serial.print(theta);
  Serial.println();
  delay(250);
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
      long x, y;
      float theta;
      odometryInterface.getOdometry(&x, &y, &theta); // TODO: CURRENTLY ONLY CAN SUPPORT FLOATS!
      response->commandID = static_cast<int>(ArduinoToPiRsp::ODOMETRY);
      response->arg1 = (float) x;
      response->arg2 = (float) y;
      response->arg3 = theta;
      response->seqNum = request->seqNum;
      return true;

    // GET_TICKS command 
    case static_cast<int>(PiToArduinoCmd::GET_TICKS):
      long left, right;
      odometryInterface.getTickCounts(&left, &right);
      response->commandID = static_cast<int>(ArduinoToPiRsp::TICKS);
      response->arg1 = (float) left;
      response->arg2 = (float) right;
      response->seqNum = request->seqNum;
      return true;

    // Unrecognized/unsupported commandID
    default: 
      response->commandID = static_cast<int>(ArduinoToPiRsp::UNRECOGNIZED_COMMAND);
      response->seqNum = request->seqNum;
      return true;
  }
}
