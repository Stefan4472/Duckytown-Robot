from enum import Enum
from math import pi as PI
import cv
import time
from PIL import Image 
from navigator import TurnType

# States that the driver can be in.
class DriveState(Enum):
  STOPPED = 1
  #WAITING_FOR_GREEN = 2
  ROLLING_INTO_INTERSECTION = 2
  IN_INTERSECTION = 3
  FOLLOWING_LANE = 4
  APPROACHING_STOP = 5
  
LEFT_TURN_RADIUS = 14.0
RIGHT_TURN_RADIUS = 5.0

ROLL_DIST_CM = 5.0

class Driver:
  def __init__(self, arduino_interface):
    self.car = arduino_interface
    self.speed_limit = 10.0
    
    self.state = DriveState.STOPPED
    
    self.needs_instruction = True
    self.next_turn = None
    
    self.red_roi = cv.red_roi
    self.green_roi = cv.green_roi
    
  def update(self, image):
    if self.needs_instruction:
      print('Driver needs instruction!')
      return
      
    # Do nothing if in the intersection--will just be waiting for
    # the openloop command to finish.
    if self.state == DriveState.ROLLING_INTO_INTERSECTION or
       self.state == DriveState.IN_INTERSECTION:
      return
      
    # Look for Stop using RegionOfInterest code
    red = cv.search_region(image, self.red_roi, cv.Color.RED)
    
    # TODO: SIMPLIFY CODE: HAVE ONE IF STATEMENT FOR EACH STATE
    
    # Stop line seen
    if red:
      # Currently following lane: transition to APPROACHING_STOP
      if self.state == DriveState.FOLLOWING_LANE:
        self.state = DriveState.APPROACHING_STOP
        print('Stop seen for the first time')
        
        # Debugging
        cv.draw_region(image, self.red_roi, (255, 255, 255))
        cv.draw_square(image, self.red_roi.get_center()[0], self.red_roi.get_center()[1], (255, 255, 255), 5)
        Image.fromarray(image, 'RGB').show()
      
      # Currently approaching stop: track the stop line as it gets closer
      if self.state == DriveState.APPROACHING_STOP:
        print('APPROACHING STOP')
        # Get stop center.
        start_red, end_red = cv.get_color_extent_vt(image, red[0], red[1])  # TODO: GET WHOLE REGION OF RED?
        red_center = start_red[0] + int((end_red[0] - start_red[0]) / 2.0), red[1]
        
        # Center red ROI on the center of the red line
        self.red_roi.recenter(red_center[0], red_center[1])
        print('Centered roi to {}, {}'.format(red_center[0], red_center[1]))
        
        # Stop if the center is below a certain line of the image
        if self.red_roi.get_center()[0] > 120:
          self.car.command_motor_pwms(0, 0)
          self.state = DriveState.STOPPED
          # Center green ROI to the center of the red ROI
          self.green_roi.recenter(self.red_roi.get_center()[0], self.red_roi.get_center()[1])
          self.needs_instruction = True
          
          # Debugging
          Image.fromarray(image, 'RGB').show()
          
    # Currently stopped: check for green.
    if self.state == DriveState.STOPPED and not self.needs_instruction:
      # Carefully search for green
      green = cv.search_region(image, self.green_roi, cv.Color.GREEN, box_size=1, step_rows=1, step_cols=1)
      
      # Green seen: start rolling into intersection
      if green:
        self.state = DriveState.ROLLING_INTO_INTERSECTION
        self.car.command_openloop_straight(self.speed_limit, \
            ROLL_DIST_CM, listener=self._on_openloop_finished)
        print('Found green')
        # Reset red ROIs
        self.red_roi = cv.red_roi
        
        # Debugging
        cv.draw_square(image, green[0], green[1], (0, 0, 0), 10)
        cv.draw_region(image, self.red_roi, (255, 255, 255))
        cv.draw_square(image, self.red_roi.get_center()[0], self.red_roi.get_center()[1], (255, 255, 255), 5)
        cv.draw_region(image, self.green_roi, (255, 255, 0))
        Image.fromarray(image, 'RGB').show()
        return

    
    # Get lane center coordinates *in pixels*
    yellow, white, _ = cv.analyze_img(image)

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
    
  def _on_openloop_finished(self):
    # Rolling is finished: command the next turn
    if self.state == DriveState.ROLLING_INTO_INTERSECTION:
      self.state = DriveState.IN_INTERSECTION
      
      if self.next_turn == TurnType.LEFT:
        self.car.command_openloop_lcurve(\
          self.speed_limit, LEFT_TURN_RADIUS, PI / 2.0)
      elif turn == TurnType.RIGHT:
        self.car.command_openloop_rcurve(\
          self.speed_limit, RIGHT_TURN_RADIUS, PI / 2.0)
      elif turn == TurnType.STRAIGHT:
        self.car.command_openloop_straight(self.speed_limit, 10.0)
      else:
        raise ValueError('next_turn is invalid (must be of type \'TurnType\')')
    # Intersection turn is finished: start lane following
    elif self.state == DriveState.IN_INTERSECTION:
      self.state = DriveState.FOLLOWING_LANE
      
  def instruct(self, turn):
    if self.needs_instruction:
      self.next_turn = turn
      self.needs_instruction = False
    else:
      raise Exception('Doesn\'t want an instruction')
    
  def set_speed_limit(self, speed_limit):
    if speed_limit != self.speed_limit:
      self.speed_limit = speed_limit
      self.car.set_speed_limit(speed_limit)
