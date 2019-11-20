from enum import Enum
from math import pi as PI
import cv
import time
from PIL import Image 
from navigator import TurnType

# States that the driver can be in.
class DriveState(Enum):
  IN_INTERSECTION = 1
  FOLLOWING_LANE = 2
  APPROACHING_STOP = 3
  STOPPED = 4

# TODO: DETERMINE CORRECT RADII
LEFT_TURN_RADIUS = 10.0
RIGHT_TURN_RADIUS = 5.0

class Driver:
  def __init__(self, arduino_interface):
    self.car = arduino_interface
    self.speed_limit = 10.0
    self.state = DriveState.STOPPED
    self.red_roi = cv.red_roi
    self.green_roi = cv.green_roi
    self.needs_instruction = True

  def update(self, image):
    if self.needs_instruction:
      print('Driver needs instruction!')
      return
      
    #start_time = time.time()
    
    # Get center coordinates *in pixels*
    yellow, white, stop = cv.analyze_img(image)
    
    # Get stop using RegionOfInterest code
    red = cv.search_region(image, self.red_roi, cv.Color.RED)
    
    #analyze_time = time.time()
    #print('Analyzing image took {}'.format(analyze_time - start_time))
    #print(yellow, white, stop)
  
    # Stop seen while driving.
    if red and self.state != DriveState.APPROACHING_STOP:
      self.state = DriveState.APPROACHING_STOP
      print('Stop seen for the first time')
      cv.draw_region(image, self.red_roi, (255, 255, 255))
      cv.draw_square(image, self.red_roi.get_center()[0], self.red_roi.get_center()[1], (255, 255, 255), 5)
      Image.fromarray(image, 'RGB').show()
      return
    # Stop seen and in APPROACHING_STOP: track red
    elif red:
      print('APPROACHING STOP')
      # Get stop center.
      start_red, end_red = cv.get_color_extent_vt(image, red[0], red[1])
      red_center = start_red[0] + int((end_red[0] - start_red[0]) / 2.0), red[1]
      # Center red region on the center of the red line
      self.red_roi.recenter(red_center[0], red_center[1])
      print('Centered roi to {}, {}'.format(red_center[0], red_center[1]))
      
      # Stop if the center is below a certain line of the image
      if self.red_roi.get_center()[0] > 120:
        self.car.command_motor_pwms(0, 0)
        self.state = DriveState.STOPPED
        self.needs_instruction = True
        
        # DEBUGGING FOR NOW
        green = cv.search_region(image, self.green_roi, cv.Color.GREEN, box_size=1, step_rows=1, step_cols=1)
        if green:
          print('Found green')
          cv.draw_square(image, green[0], green[1], (0, 0, 0), 10)
        else:
          print('Did not see green')
        cv.draw_region(image, self.red_roi, (255, 255, 255))
        cv.draw_square(image, self.red_roi.get_center()[0], self.red_roi.get_center()[1], (255, 255, 255), 5)
        # Center green ROI to center of red ROI
        self.green_roi.recenter(self.red_roi.get_center()[0], self.red_roi.get_center()[1])
        cv.draw_region(image, self.green_roi, (255, 255, 0))
        Image.fromarray(image, 'RGB').show()
        # Reset red ROI
        self.red_roi = cv.red_roi
        return
        
    '''
    # Look for green if STOPPED
    if self.state == DriveState.STOPPED:  # TODO: BETTER STATE MACHINE
      green = cv.search_region(image, self.green_roi, cv.Color.GREEN, step_rows = 5, step_cols=5)
      if green:
        print('Found green')
        cv.draw_region(image, self.green_roi, (255, 255, 255))
        cv.draw_square(image, green[0], green[1], (0, 0, 0), 10)
        Image.fromarray(image, 'RGB').show()
        self.green_roi = cv.green_roi
      #  self.state = 
    '''
    
    '''
    # Get lane center
    lane_center = None
    if yellow and white:
      #print('Got yellow {}, white {}'.format(yellow, white))
      lane_center = (yellow[0] + white[0]) / 2.0, (yellow[1] + white[1]) / 2.0
    elif white and not yellow:
      #print('Using white line')
      lane_center = (white[0], white[1] - cv.white_width - 1*(cv.LANE_WIDTH_PX / 2.0))
    elif yellow and not white:
      #print('Using yellow line')
      lane_center = (yellow[0], yellow[1] + cv.yellow_width + 1*(cv.LANE_WIDTH_PX / 2.0))
    
    # No lane seen while following lane: use previous point.
    if lane_center is None and self.state == DriveState.FOLLOWING_LANE:
      #print('Couldn\'t see the lane')  # TODO: TIMEOUT IF CAN'T SEE THE LANE FOR MORE THAN X SECONDS
      lane_center = self.old_ctr
    # No lane seen while in intersection: do nothing (let open loop continue)
    elif lane_center is None and self.state == DriveState.IN_INTERSECTION:
      print('Continuing open-loop')
      return
    # If lane is seen while in INTERSECTION state, move to lane_following
    elif lane_center and self.state == DriveState.IN_INTERSECTION:
      self.state = DriveState.FOLLOWING_LANE
      
    self.old_ctr = lane_center
    
    # Resolve target in robot frame.
    r_target = cv.get_position(lane_center[0], lane_center[1])
    # Send command.
    self.car.command_closedloop(r_target[0], r_target[1], 0.0)
    '''
    
  def instruct(self, turn):
    if not self.needs_instruction:
      raise Exception('Must be stopped')

    # Apply openloop command based on the TurnType.
    if turn == TurnType.LEFT:
      self.car.command_openloop_lcurve(\
        self.speed_limit, LEFT_TURN_RADIUS, PI / 2.0)
    elif turn == TurnType.RIGHT:
      self.car.command_openloop_rcurve(\
        self.speed_limit, RIGHT_TURN_RADIUS, PI / 2.0)
    elif turn == TurnType.STRAIGHT:
      #self.car.command_openloop_straight(self.speed_limit)
      #print('Commanded openloop straight')
      pass
    else:
      raise ValueError('Invalid turn (must be of type TurnType)')
    self.needs_instruction = False
    self.state = DriveState.IN_INTERSECTION
    
  def set_speed_limit(self, speed_limit):
    if speed_limit != self.speed_limit:
      self.car.set_speed_limit(speed_limit)
      self.speed_limit = speed_limit
