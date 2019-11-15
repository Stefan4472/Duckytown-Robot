from enum import Enum
from math import pi as PI
import cv
import time
from PIL import Image 

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
    self.state = DriveState.FOLLOWING_LANE
    self.speed_limit = 10.0
    return

  # Returns False if it requires more instructions.
  def update(self, image):
    if self.state == DriveState.STOPPED:
      print('Stopped')
      input("Press Enter to continue")
      self.state = DriveState.FOLLOWING_LANE
      return
    print('Updating driver')
    # Do nothing if stopped. Await instructions.
    #if self.state == DriveState.STOPPED:
    #  return False
      
    # TODO: IMAGE PROCESSING. Switch to lane-following once lanes are seen
    
    start_time = time.time()
    # Get center coordinates *in pixels*
    yellow, white, stop = cv.analyze_img(image)
    analyze_time = time.time()
    print('Analyzing image took {}'.format(analyze_time - start_time))
    print(yellow, white, stop)
    # Get lane center
    lane_center = 0
    if stop and stop[1] < cv.img_width*0.77 and stop[1] > cv.img_width*0.33:
      self.car.command_motor_pwms(0, 0)
      self.state = DriveState.STOPPED
      return
    if yellow and white:
      print('Got yellow {}, white {}'.format(yellow, white))
      lane_center = (yellow[0] + white[0]) / 2.0, (yellow[1] + white[1]) / 2.0
    elif white and not yellow:
      print('Using white line')
      lane_center = (white[0], white[1] - 1.3*(cv.LANE_WIDTH_PX / 2.0))
    elif yellow and not white:
      print('Using yellow line')
      lane_center = (yellow[0], yellow[1] + 0.8*(cv.LANE_WIDTH_PX / 2.0))
    else:
      print('Couldn\'t see the lane')
      lane_center = self.old_ctr
      #Image.fromarray(image, 'RGB').show()
      #raise Exception('Can\'t see the road')
      #return
    self.old_ctr = lane_center
    print('Lane center at px({})'.format(lane_center))
    # Resolve target in robot frame
    r_target = cv.get_position(lane_center[0], lane_center[1])
    print('Resolved to target r({})'.format(r_target))
    start_time = time.time()
    self.car.command_closedloop(r_target[0], r_target[1], 0.0)
    print('Sending the command took {}'.format(time.time() - start_time))
    print('Sent command')
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
