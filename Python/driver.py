from enum import Enum
from math import pi as PI

# States that the driver can be in.
class DriveState(Enum):
  IN_INTERSECTION = 1
  FOLLOWING_LANE = 2
  APPROACHING_STOP = 3
  STOPPED = 4

class TurnType(Enum):
  LEFT = 1,
  RIGHT = 2,
  STRAIGHT = 3

# TODO: DETERMINE CORRECT RADII
LEFT_TURN_RADIUS = 10.0
RIGHT_TURN_RADIUS = 5.0


class Driver:
  def __init__(self, arduino_interface):
    self.car = arduino_interface
    self.state = DriveState.STOPPED
    self.speed_limit = 10.0
    return

  # Returns False if it requires more instructions.
  def update(self, image):
    # Do nothing if stopped. Await instructions.
    if self.state == DriveState.STOPPED:
      return False
      
    # TODO: IMAGE PROCESSING. Switch to lane-following once lanes are seen
    return True

  def instruct(self, turn):
    if self.state != DriveState.STOPPED:
      raise Exception('Must be stopped')

    # Apply openloop command based on the turnType.
    if turn == TurnType.LEFT:
      self.car.command_openloop_lcurve(\
        self.speed_limit, LEFT_TURN_RADIUS, PI / 2.0)
    elif turn == TurnType.RIGHT:
      self.car.command_openloop_rcurve(\
        self.speed_limit, RIGHT_TURN_RADIUS, PI / 2.0)
    elif turn == TurnType.STRAIGHT:
      self.car.command_openloop_straight(self.speed_limit)
    else:
      raise ValueError('Invalid turn (must be of type TurnType)')

    # Move to INTERSECTION state
    self.state = DriveState.IN_INTERSECTION

  def set_speed_limit(self, speed_limit):
    if speed_limit != self.speed_limit:
      self.car.command_speed_limit(speed_limit)  # TODO: IMPLEMENT
      self.speed_limit = speed_limit