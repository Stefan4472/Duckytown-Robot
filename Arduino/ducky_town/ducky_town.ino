#include <math.h>
#include "packet_defs.h"
#include "serial_interface.h"
#include "odometry_interface.h"
#include "wheel_interface.h"
#include "icontroller.h"
#include "open_loop_controller.h"
#include "closed_loop_controller.h"

SerialInterface serialInterface;
WheelInterface wheelInterface;
OdometryInterface odometryInterface;
OpenLoopController openLoopController;
ClosedLoopController closedLoopController;
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
//  closedLoopController.onConvergedFcn = onClosedLoopConverged;
//  closedLoopController.commandPosition(70 + 12, 70, PI/2.0);
//  closedLoopController.commandPosition(200 + 12, 0, 0);
//  closedLoopController.commandPosition(12, 0, PI/2.0);
//  currController = &closedLoopController;
}

 int ms_since_print = 0;
 bool test_started = false;
 bool test_finished = false;
 unsigned long start_time;
 int currIndex = 0;
 int totalIndexes = 4;
 float circlePoints[12][3] = {
    {9,2.4,PI/6.0},
    {16,9,2*PI/6.0},
    {18,18,3*PI/6.0},
    {16,27,4*PI/6.0},
    {9,33.6,5*PI/6.0},
    {0,36,PI},
    {-9,33.6,7*PI/6.0},
    {-16,27,8*PI/6.0},
    {-18,18,9*PI/6.0},
    {-16,9,10*PI/6.0},
    {-9,2.4,11*PI/6.0},
    {0,0,0}
  };

//float circlePoints[4][3] = {
//    {30, 0, 0},
//    {40, 0, 0},
//    {50, 0, 0},
//    {60, 0, 0}
//  };

void onClosedLoopConverged()
{
  Serial.println("ClosedLoopConverged");
  Serial.println("x " + String(odometryInterface.x) + " y " + String(odometryInterface.y) + " t " + String(odometryInterface.theta));
//  Serial.println("ClosedLoopConverged on index " + String(currIndex));
//  currIndex++;
//  if (currIndex == totalIndexes)
//  {
//    Serial.println("Finished");
//  }
//  else 
//  {
//    int i = currIndex % 4;
//    closedLoopController.commandPosition(circlePoints[i][0], circlePoints[i][1], circlePoints[i][2]);
//    currController = &closedLoopController;
//    currController->finished = false;
//  }
}

bool converged = false;
void loop()
{
  static PiToArduinoPacket recv_packet;
  static ArduinoToPiPacket send_packet;
  static unsigned long curr_time;

 
  curr_time = millis();

//  float radius = 55.0;
//  
//  float theta = 2.0 * PI * (curr_time / 6000.0);
//  float x = 12 + radius * sin(theta);
//  float y = radius - radius * cos(theta);
//  Serial.println("x: " + String(x) + " y: " + String(y) + " t: " + String(theta));
//  closedLoopController.commandPosition(x, y, theta);
//  if (!test_started)
//  {
//    closedLoopController.commandPosition(circlePoints[0][0], circlePoints[0][1], circlePoints[0][2]);
//    currController = &closedLoopController;
//    // 50 cm straight
////    wheelInterface.setSpeedLimit(10.0);
////    closedLoopController.commandPosition(112, 0, 0);
////    currController = &closedLoopController;
////    if(closedLoopController.finished || test_started == false){
////      closedLoopController.commandPosition(circlePoints[cycleNum][0], circlePoints[cycleNum][1], cycleNum*M_PI/6.0);
////      cycleNum++;
////    }
//    test_started = true;
//    converged = false;
//  }
////  if (test_finished && Serial.available())
////   {
////     test_finished = false;
////     test_started = false;
////     while (Serial.available())
////     {
////        Serial.read();
////     }
////   }
////  
////   if (!test_started)
////   {
////     start_time = millis();
////     Serial.....println("Test started at " + String(start_time));
////     odometryInterface.resetTo(0, 0, 0);
////     //wheelInterface.commandPWMs(350, 350);
////     wheelInterface.commandStraightSpeed(10.0);
////     test_started = true;
////   }
////  
////   if (test_started && !test_finished && curr_time - start_time >= 10000)
////   {
////     test_finished = true;
////     wheelInterface.commandPWMs(0, 0);
////     Serial.println("Took " + String(millis() - start_time) + " ms");
////     long left, right;
////     odometryInterface.getTickCounts(&left, &right);
////     Serial.println("Left: " + String(left) + " Right: " + String(right));
////     Serial.println("x: " + String(odometryInterface.x) + " y: " + String(odometryInterface.y) + " t: " + String(odometryInterface.theta));
////   }

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

// if (currController && currController->finished)
// {
//  Serial.println("ClosedLoopConverged on index " + String(currIndex));
//  currIndex++;
//  if (currIndex == totalIndexes)
//  {
//    Serial.println("Finished");
//    currController->finished = true;
//  }
//  else 
//  {
//    int i = currIndex % 12;
//    closedLoopController.commandPosition(circlePoints[i][0], circlePoints[i][1], circlePoints[i][2]);
//    currController = &closedLoopController;
//    currController->finished = false;
//  }
// }
//
// if (!currController)
// {
//  Serial.println("No controller");
// }
// if (currController && currController->finished)
// {
//  Serial.println("Controller set, but finished");
// }
 // Update controller, if one is set.
 if (currController && !currController->finished)
 {
   currController->update(&odometryInterface, &wheelInterface);
 }

 // Print readings once every 1000 ms.
 ms_since_print += curr_time - lastLoop;
 if (ms_since_print > 250)
 {
   long left, right;
   odometryInterface.getTickCounts(&left, &right);
  float yoke_x = odometryInterface.x + CHASSIS_LENGTH_CM * cos(odometryInterface.theta);
  float yoke_y = odometryInterface.y + CHASSIS_LENGTH_CM * sin(odometryInterface.theta);
  float yoke_t = odometryInterface.theta;
  Serial.println("Yoke at " + String(yoke_x) + ", " + String(yoke_y) + ", " + String(yoke_t));
    
   Serial.print("World-frame position: ");
   Serial.print(odometryInterface.x);
   Serial.print(" - ");
   Serial.print(odometryInterface.y);
   Serial.print(" - ");
   Serial.print(odometryInterface.theta);
   Serial.println();
   Serial.println("Estimated dx " + String(odometryInterface.dX) + " dy " + String(odometryInterface.dY) + " dt " + String(odometryInterface.dTheta));
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

    // SET_CLOSEDLOOP command
    case static_cast<int>(PiToArduinoCmd::SET_CLOSEDLOOP):
      //      Serial.println("Running left curve, target = " + String(request->arg3));
      closedLoopController.commandPosition(request->arg1, request->arg2, request->arg3); //, &wheelInterface, &odometryInterface);
      Serial.println("Turning on closed-loop control");
      currController = &closedLoopController;
      return false;

    // Unrecognized/unsupported commandID
    default:
      response->commandID = static_cast<int>(ArduinoToPiRsp::UNRECOGNIZED_COMMAND);
      response->seqNum = request->seqNum;
      return true;
  }
}
