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
  
LEFT_TURN_RADIUS = 14.0
RIGHT_TURN_RADIUS = 5.0

ROLL_DIST_CM = 22.0 #5

class Driver:
  def __init__(self, arduino_interface):
    self.car = arduino_interface
    self.speed_limit = 10.0
    
    # Diagnostics
    self.num_updates = 0
    self.start_time = None

    self.set_state(DriveState.FOLLOWING_LANE)  # TODO: START IN LANE_FOLLOWING?
    #self.set_state(DriveState.STOPPED)  # TODO: START IN LANE_FOLLOWING?
    
    self.needs_instruction = True
    self.next_turn = None
    
    self.red_roi = cv.red_roi
    self.green_roi = cv.green_roi
    self.green_roi.recenter(*self.red_roi.get_center())
    
  def instruct(self, turn):
    if self.needs_instruction:
      self.next_turn = turn
      self.needs_instruction = False
    else:
      raise Exception('Doesn\'t want an instruction')
    
  def set_speed_limit(self, speed_limit):
    if speed_limit != self.speed_limit:
      self.speed_limit = speed_limit
      # self.car.set_speed_limit(speed_limit)

  def set_state(self, new_state):
    print('STATE CHANGE to {}'.format(new_state))

    if self.start_time:
      print('Average control rate for {} was {}'.format( \
          self.state, self.num_updates / (time.time() - self.start_time)))

    self.state = new_state
    self.start_time = time.time()
    self.num_updates = 0

  # Debugging
  def debug_image(self, image):
    cv.draw_region(image, self.red_roi, (255, 255, 255))
    cv.draw_region(image, self.green_roi, (255, 255, 0))
    Image.fromarray(image, 'RGB').show()

  def update(self, image):
    if self.needs_instruction:
      print('Driver needs instruction!')
      return
      
    self.num_updates += 1
    # print('Driver in state {}'.format(self.state))
    
    # LANE FOLLOWING
    if self.state == DriveState.FOLLOWING_LANE:
      # Look for Stop using RegionOfInterest code
      red = cv.search_region(image, self.red_roi, cv.Color.RED)
      
      # Stop line seen
      if red:
        print('Stop seen')
        # Get stop center (along vertical axis).
        start_red, end_red = cv.get_color_extent_vt(image, red[0], red[1])
        red_center = start_red[0] + int((end_red[0] - start_red[0]) / 2.0), red[1]
        # Center green ROI on the center of the red line.
        self.green_roi.recenter(*red_center)
        # Stop the car
        self.car.command_motor_pwms(0, 0)
        self.set_state(DriveState.STOPPED)
        return
      
      # Stop not seen: continue lane-following.
      # Find pixel coordinates of yellow and white features.
      yellow, white, _ = cv.analyze_img(image)

      # Get lane center
      lane_center = None
      if yellow and white:
        print('Got yellow {}, white {}'.format(yellow, white))
        lane_center = (yellow[0] + white[0]) / 2.0, (yellow[1] + white[1]) / 2.0
      elif white and not yellow:
        print('Using white line')
        lane_center = (white[0], white[1] - cv.white_width - 1*(cv.LANE_WIDTH_PX / 2.0))
      elif yellow and not white:
        print('Using yellow line')
        lane_center = (yellow[0], yellow[1] + cv.yellow_width + 10 + 1*(cv.LANE_WIDTH_PX / 2.0))
      
      # No lane seen while following lane: use previous point.
      if lane_center is None:
        print('Couldn\'t see the lane')  # TODO: TIMEOUT IF CAN'T SEE THE LANE FOR MORE THAN X SECONDS?
        try:
          lane_center = self.old_ctr
        except:
          lane_center = (0, 160)
        
      self.old_ctr = lane_center
      
      # Resolve target in robot frame.
      r_target = cv.get_position(*lane_center)
      # Send command.
      self.car.command_closedloop(self.speed_limit, r_target[1])
      
    # Currently stopped: check for green.  
    elif self.state == DriveState.STOPPED:
      #print('Searching for green in {}'.format(self.green_roi))
      green = cv.search_region(image, self.green_roi, cv.Color.GREEN, box_size=1, step_rows=2, step_cols=2)
      
      # Green seen: start rolling into intersection
      if green:
        print('Found green')
        #self.debug_image(image)
        #self.car.command_openloop_straight(self.speed_limit, \
        #    ROLL_DIST_CM, self._on_openloop_finished)     # TODO: THIS WILL GET CEILINGED BY THE OVERALL SPEEDLIMIT
        self.car.reset_odometry()
        self.next_turn = TurnType.LEFT
        rdc = 31 if self.next_turn == TurnType.LEFT else 24 # ROLL_DIST_CM, but better
        try:
          self.next_turn
        except:
          return
        self.wd_timer = time.time() + 4.5
        self.set_state(DriveState.ROLLING_INTO_INTERSECTION)
        self.car.command_openloop_straight(20.0, \
            rdc, self._on_openloop_finished) # CHANGED FROM ROLL_DIST_CM TO rdc
        
        print('Rolling command sent at {}'.format(time.time()))
        
        
    # ROLLING_INTO_INTERSECTION
    # Do nothing: wait for openloop to finish
    elif self.state == DriveState.ROLLING_INTO_INTERSECTION:
      if self.wd_timer < time.time():
        print("\n! Watchdog timer exceeded. Proceed to Intersection state.\n")
        self._on_openloop_finished(0,0,0)
        self.wd_timer = time.time() + 4.5
      pass
      
    # IN_INTERSECTION
    # # Do nothing: wait for openloop to finish
    elif self.state == DriveState.IN_INTERSECTION:
      if self.wd_timer < time.time():
        print("\n! Watchdog timer exceeded. Proceed to lane following state.\n")
        self._on_openloop_finished(0,0,0)
      #print("INTERSECTING")
      pass
      
    else:
      raise Exception('Invalid State: {}'.format(self.state))
    
  def _on_openloop_finished(self, arg1, arg2, arg3):
    print('{} GOT OPENLOOP CALLBACK, state={}'.format(time.time(), self.state))
    # Rolling is finished: command the next turn
    if self.state == DriveState.ROLLING_INTO_INTERSECTION:
      print('Received openloop callback while ROLLING_INTO_INTERSECTION')
      self.set_state(DriveState.IN_INTERSECTION)
      #print('SLEEPING')
      #time.sleep(5.0)
      
      if self.next_turn == TurnType.LEFT:
        self.car.command_openloop_lcurve(self.speed_limit, \
            LEFT_TURN_RADIUS, 1.2*PI / 2.0, self._on_openloop_finished)
      elif self.next_turn == TurnType.RIGHT:
        self.car.command_openloop_rcurve(self.speed_limit, \
            RIGHT_TURN_RADIUS+5.0, PI / 4.0, self._on_openloop_finished)
      elif self.next_turn == TurnType.STRAIGHT:
        self.car.command_openloop_straight(self.speed_limit, \
            20.0, self._on_openloop_finished)
      else:
        raise ValueError('next_turn is invalid (must be of type \'TurnType\')')
    # Intersection turn is finished: start lane following
    elif self.state == DriveState.IN_INTERSECTION:
      print('Received openloop callback while IN_INTERSECTION')
      self.set_state(DriveState.FOLLOWING_LANE)

