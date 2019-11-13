from enum import Enum
from math import pi as PI
import cv

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
    print('Updating driver')
    # Do nothing if stopped. Await instructions.
    #if self.state == DriveState.STOPPED:
    #  return False
      
    # TODO: IMAGE PROCESSING. Switch to lane-following once lanes are seen
    
    # Get center coordinates *in pixels*
    yellow, white, stop = cv.analyze_img(image)
    print(yellow, white, stop)
    # Get lane center
    lane_center = 0
    if yellow and white:
      print('Got yellow {}, white {}'.format(yellow, white))
      lane_center = (yellow[0] + white[0]) / 2.0, (yellow[1] + white[1]) / 2.0
      print('Lane center at px({})'.format(lane_center))
      # Resolve target in robot frame
      r_target = cv.get_position(lane_center[0], lane_center[1])
      print('Resolved to target r({})'.format(r_target))
      self.car.command_closedloop(r_target[0], r_target[1], 0.0)
      print('Sent command')
    else:
      print('Couldn\'t see the lane')
    #elif found_y:
    #  lane_center = yellow_loc[1] + yellow_width + (lane_width / 2)
    #elif found_w:
    #  lane_center = white_loc[1] - (lane_width / 2)
    
    #y_off = (lane_center - center) / pixels_per_cm
    
    
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
      #self.car.command_speed_limit(speed_limit)  # TODO: IMPLEMENT
      self.speed_limit = speed_limit
