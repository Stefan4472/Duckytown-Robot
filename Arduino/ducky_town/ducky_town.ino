#include <math.h>
#include "packet_defs.h"
#include "serial_interface.h"
#include "odometry_interface.h"
#include "wheel_interface.h"
#include "icontroller.h"
#include "open_loop_controller.h"

SerialInterface serialInterface;
WheelInterface wheelInterface;
OdometryInterface odometryInterface;
OpenLoopController openLoopController;
IController* currController;

// TODO: RUN AT A HIGHER BAUD RATE
const long BAUD_RATE = 9600;

unsigned long lastLoop;

void setup() 
{
  serialInterface.init(BAUD_RATE);
  odometryInterface.init();
  wheelInterface.init();
  lastLoop = millis();
}

int ms_since_print = 0;

void loop()
{
  static PiToArduinoPacket recv_packet;
  static ArduinoToPiPacket send_packet;
  static unsigned long curr_time;

  curr_time = millis();
  
  // Process any queued packets.
  while (serialInterface.getNextPacket(&recv_packet))
  {
    Serial.println("Got another packet");
    // Handle the request and send a response if requested.
    if (handleRequest(&recv_packet, &send_packet))
    {
      serialInterface.sendPacket(&send_packet);
    }
  }

  // Update odometry
  odometryInterface.update();
  
  // Update controller, if one is set.
  if (currController && !currController->finished)
  {
    currController->update(&odometryInterface, &wheelInterface);
  }
  
  // Print readings once every 250 ms.
  ms_since_print += curr_time - lastLoop;
  if (ms_since_print > 1000)
  {
    long left, right;
    odometryInterface.getTickCounts(&left, &right);  // TODO: ARE TICK COUNTS RESETTING EACH TIME WE CONNECT?
    Serial.println();
    Serial.print("Left/Right tick counts: ");
    Serial.print(left);
    Serial.print(" - ");
    Serial.print(right);
    Serial.println();
    Serial.print("World-frame position: ");
    Serial.print(odometryInterface.x);
    Serial.print(" - ");
    Serial.print(odometryInterface.y);
    Serial.print(" - ");
    Serial.print(odometryInterface.theta);
    Serial.println();
    ms_since_print = 0;
  }
  
  lastLoop = curr_time;
}

// TODO: MOTOR_ERROR
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
//      Serial.println("Commanding PWMs");
//      Serial.print((int) request->arg1);
//      Serial.print('/');
//      Serial.print((int) request->arg2);
//      Serial.println();
      Serial.println("Turning off open-loop control");
      // Turn off the controller (if any)
      currController = NULL;
      wheelInterface.commandPWMs((int) request->arg1, (int) request->arg2);
      return false;

    // GET_ODOMETRY command 
    case static_cast<int>(PiToArduinoCmd::GET_ODOMETRY):
      long x, y;
      float theta;
      response->commandID = static_cast<int>(ArduinoToPiRsp::ODOMETRY);
      response->arg1 = (float) odometryInterface.x;
      response->arg2 = (float) odometryInterface.y;
      response->arg3 = odometryInterface.theta;
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

    // SET_OPENLOOP_STRAIGHT command 
    case static_cast<int>(PiToArduinoCmd::SET_OPENLOOP_STRAIGHT):
      Serial.println("Running straight line, target = " + String(request->arg2));
      openLoopController.commandStraight(request->arg1, request->arg2, &wheelInterface);
      Serial.println("Turning on open-loop control");
      currController = &openLoopController;
      return false;

    // SET_OPENLOOP_R_CURVE command 
    case static_cast<int>(PiToArduinoCmd::SET_OPENLOOP_R_CURVE):
      Serial.println("Running right curve, target = " + String(request->arg3));
      openLoopController.commandRightTurn(request->arg1, request->arg2, request->arg3, &wheelInterface);
      Serial.println("Turning on open-loop control");
      currController = &openLoopController;
      return false;

    // SET_OPENLOOP_R_CURVE command 
    case static_cast<int>(PiToArduinoCmd::SET_OPENLOOP_L_CURVE):
      Serial.println("Running left curve, target = " + String(request->arg3));
      openLoopController.commandLeftTurn(request->arg1, request->arg2, request->arg3, &wheelInterface);
      Serial.println("Turning on open-loop control");
      currController = &openLoopController;
      return false;
  
    // Unrecognized/unsupported commandID
    default: 
      response->commandID = static_cast<int>(ArduinoToPiRsp::UNRECOGNIZED_COMMAND);
      response->seqNum = request->seqNum;
      return true;
  }
}
