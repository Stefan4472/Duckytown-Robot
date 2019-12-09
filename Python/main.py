import time
import numpy as np
from arduino_interface import ArduinoInterface
from driver import Driver, DriveState
from navigator import Navigator
from picam_interface import Camera
from PIL import Image

IMAGE_WIDTH = 320
IMAGE_HEIGHT = 240

segment_start_time = None
num_driver_updates = 0

cur_speed = 10

def _receive_statistics(msPerLoop, msPerPacket, numPackets):
  print('Received statistics {} ms per loop, {} ms per packet, {} packets'.format(msPerLoop, msPerPacket, int(numPackets)))
  
  
if __name__ == '__main__':
  arduino_interface = ArduinoInterface('/dev/ttyACM0', 115200, timeout=1.0)
  camera = Camera()
  driver = Driver(arduino_interface)
  ipaths = input("Enter start: ")
  ipathm = input("Enter middle: ")
  ipathe = input("Enter end: ")
  if ipaths == 7 or ipaths == 11:
    cur_speed = 15
  navigator = Navigator((int(ipaths), int(ipathm), int(ipathe)))
  #navigator = Navigator((1, 5, 10))  # TODO: PROVIDE MAP
  
  # navigator.plan_route(...)

  camera.start()
  time.sleep(2)
  driver.set_speed_limit(10.0)  # TODO: DIFFERENTIATE BETWEEN SPEED LIMIT AND DESIRED SPEED
  pixel_data = np.empty(shape=(IMAGE_HEIGHT, IMAGE_WIDTH, 3), dtype='uint8')
  #arduino_interface.turn_statistics_on(10, _receive_statistics)
  
  try:
    while True:
      # Process the serial port.
      arduino_interface.process_buffer()
      
      # Send the next instruction if driver is waiting.
      if driver.needs_instruction:
        """if segment_start_time:
          print('This segment had an average control rate of {} hz'.\
                format(num_driver_updates * 1.0 / (time.time() - segment_start_time)))
        """        
          
        # Wait for input before starting next segment (debugging)
        #input('Press enter to trigger next instruction')
        try:
          next_iturn, next_ispeed = navigator.get_next_instruction()
        except:
          print('Route finished. (or something bad happened). Exiting.')
          driver.last_stop = True
          pass
        print("Next turn: {}\nNext speed: {}".format(next_iturn, next_ispeed))
        driver.instruct(next_iturn)
        driver.set_speed_limit(cur_speed)
        cur_speed = next_ispeed
        """segment_start_time = time.time()"""
        num_driver_updates = 1
        
      # Run next control update if a frame is ready.
      if camera.frame_ready:
        #print('Frame ready at time {}'.format(time.time()))
        # start_time = time.time()
        camera.get_frame(pixel_data)
        driver.update(pixel_data)
        # end_time = time.time()
        # print('Update loop took {} seconds'.format(end_time - start_time))
        # num_driver_updates += 1
        
  finally:
    print('Turning off motors')
    arduino_interface.command_motor_pwms(0, 0)
