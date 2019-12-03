#include <math.h>
#include "packet_defs.h"
#include "serial_interface.h"
#include "odometry_interface.h"
#include "wheel_interface.h"
#include "icontroller.h"
#include "open_loop_controller.h"
#include "closed_loop_controller.h"
#include "proximity_sensor.h"
#include "lights_interface.h"

SerialInterface serialInterface;
WheelInterface wheelInterface;
OdometryInterface odometryInterface;
OpenLoopController openLoopController;
ClosedLoopController closedLoopController;
IController* currController;
ProximitySensor proximitySensor;
LightsInterface lightsInterface;

#define BAUD_RATE 115200
#define PING_PIN 6

unsigned long lastLoop;

void setup()
{
  serialInterface.init(BAUD_RATE);
  odometryInterface.init();
  wheelInterface.init();
  proximitySensor.init(PING_PIN);
  closedLoopController.init();
  openLoopController.init();
  lastLoop = millis();
  wheelInterface.setSpeedLimit(10.0);
  lightsInterface.init(14, 15);
}

void loop()
{
  static PiToArduinoPacket recv_packet;
  static ArduinoToPiPacket send_packet;
  static unsigned long curr_time;
  static int ms_since_print = 0;

  // Process any queued packets.
  while (serialInterface.getNextPacket(&recv_packet))
  {
    // Serial.println("Got packet at " + String(millis()));
    // Handle the request and send a response if requested.
    if (handleRequest(&recv_packet, &send_packet))
    {
      serialInterface.sendPacket(&send_packet);
    }
  }

  curr_time = millis();

  // Update odometry
  odometryInterface.update();

  // Update controller, if one is set.
  if (currController)
  {
    currController->update(&odometryInterface, &wheelInterface, &lightsInterface);

    // If finished, clear the reference.
    if (currController->finished)
    {
      currController = NULL;
    }
  }

  // Check distance and limit speed, if necessary
  proximitySensor.runCollisionAvoidance(odometryInterface.dX, &wheelInterface);

  // Update wheel interface.
  wheelInterface.update(&lightsInterface);
  
  // Update lights
  lightsInterface.update();

  // Print readings once every 1000 ms.
  ms_since_print += curr_time - lastLoop;
  if (ms_since_print > 500)
  {
    //float dist = proximitySensor.getDistCm();
    //Serial.println("Proximity sensor reads " + String(dist) + " cm");
    ms_since_print = 0;
  }

  lastLoop = curr_time;
}

// TODO: MOTOR_ERROR?
// Handles receiving a PiToArduinoPacket (request). Fills in the 'response' 
// packet and returns whether it should be sent.
bool handleRequest(PiToArduinoPacket* request, ArduinoToPiPacket* response)
{
  // Handle the possible commandIDs.
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

    // SET_SPEEDLIMIT command
    case static_cast<int>(PiToArduinoCmd::SET_SPEEDLIMIT):
      wheelInterface.setSpeedLimit(request->arg1);
      return false;
      
    // SET_MOTORS command
    case static_cast<int>(PiToArduinoCmd::SET_MOTORS):
      // Turn off the controller (if any)
      currController = NULL;
      wheelInterface.commandPWMs((int) request->arg1, (int) request->arg2);
      return false;

    // GET_ODOMETRY command
    case static_cast<int>(PiToArduinoCmd::GET_ODOMETRY):
      response->commandID = static_cast<int>(ArduinoToPiRsp::ODOMETRY);
      response->arg1 = odometryInterface.x;
      response->arg2 = odometryInterface.y;
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
      openLoopController.commandStraight(request->arg1, request->arg2, &wheelInterface);
      currController = &openLoopController;
      return false;

    // SET_OPENLOOP_R_CURVE command
    case static_cast<int>(PiToArduinoCmd::SET_OPENLOOP_R_CURVE):
      openLoopController.commandRightTurn(request->arg1, request->arg2, request->arg3, &wheelInterface, &lightsInterface);
      lightsInterface.startRightBlinker();
      currController = &openLoopController;
      return false;

    // SET_OPENLOOP_L_CURVE command
    case static_cast<int>(PiToArduinoCmd::SET_OPENLOOP_L_CURVE):
      openLoopController.commandLeftTurn(request->arg1, request->arg2, request->arg3, &wheelInterface, &lightsInterface);
      lightsInterface.startRightBlinker();
      currController = &openLoopController;
      return false; 

    // SET_CLOSEDLOOP command  TODO: CURRENTLY ONLY USES THE Y PARAMETER
    case static_cast<int>(PiToArduinoCmd::SET_CLOSEDLOOP):
      closedLoopController.commandPosition(12.0 + odometryInterface.x + request->arg1, -request->arg2,
                                           request->arg3);
      currController = &closedLoopController;
      return false;

    // Unrecognized/unsupported commandID
    default:
      response->commandID = static_cast<int>(ArduinoToPiRsp::UNRECOGNIZED_COMMAND);
      response->seqNum = request->seqNum;
      return true;
  }
}
