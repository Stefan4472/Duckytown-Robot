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

def _receive_statistics(msPerLoop, msPerPacket, arg3):
  print('Received statistics {} ms per loop, {} ms per packet'.format(msPerLoop, msPerPacket))
  
  
if __name__ == '__main__':
  arduino_interface = ArduinoInterface('/dev/ttyACM0', 115200, timeout=1.0)
  camera = Camera(framerate=20)
  driver = Driver(arduino_interface)
  navigator = Navigator((1, 5, 10))  # TODO: PROVIDE MAP
  # navigator.plan_route(...)

  camera.start()
  time.sleep(2)
  driver.set_speed_limit(10.0)
  pixel_data = np.empty(shape=(IMAGE_HEIGHT, IMAGE_WIDTH, 3), dtype='uint8')
  #arduino_interface.turn_statistics_on(10, _receive_statistics)
  
  try:
    while True:
      # Process the serial port.
      arduino_interface.process_buffer()
      
      # Send the next instruction if driver is waiting.
      if driver.needs_instruction:
        if segment_start_time:
          print('This segment had an average control rate of {} hz'.\
                format(num_driver_updates * 1.0 / (time.time() - segment_start_time)))
                
        # Exit if the route is finished.
        if navigator.route_finished:
          print('Route finished. Exiting.')
          break
          
        # Wait for input before starting next segment (debugging)
        #input('Press enter to trigger next instruction')
        driver.instruct(navigator.get_next_instruction())
        segment_start_time = time.time()
        num_driver_updates = 1
        
      # Run next control update if a frame is ready.
      if camera.frame_ready:
        #print('Frame ready at time {}'.format(time.time()))
        start_time = time.time()
        camera.get_frame(pixel_data)
        driver.update(pixel_data)
        end_time = time.time()
        print('Update loop took {} seconds'.format(end_time - start_time))
        num_driver_updates += 1
        
  finally:
    print('Turning off motors')
    arduino_interface.command_motor_pwms(0, 0)
