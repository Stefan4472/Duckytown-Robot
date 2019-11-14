import time
import numpy as np
from arduino_interface import ArduinoInterface
from driver import Driver, TurnType
from picam_interface import Camera
from PIL import Image

IMAGE_WIDTH = 320
IMAGE_HEIGHT = 240

if __name__ == '__main__':
  arduino_interface = ArduinoInterface('/dev/ttyACM0', 115200, timeout=1.0)
  camera = Camera(framerate=10)
  driver = Driver(arduino_interface)
  # navigator = Navigator()

  camera.start()
  time.sleep(2)
  driver.set_speed_limit(15.0)
  #driver.instruct(TurnType.STRAIGHT)
  pixel_data = np.empty(shape=(IMAGE_HEIGHT, IMAGE_WIDTH, 3), dtype='uint8')
  
  '''
  while not camera.frame_ready:
    continue
  print('\nGot next frame')
  camera.get_frame(pixel_data)
  driver.update(pixel_data)
  Image.fromarray(pixel_data, 'RGB').show()
  
  while 1:
    while arduino_interface.serial_port.in_waiting:
      print(arduino_interface.serial_port.readline())
  '''
  
  try:
    while True:
      arduino_interface.serial_port.flush()
      if camera.frame_ready:
        start_time = time.time()
        print('\nGot next frame, t={}'.format(time.time()))
        while arduino_interface.serial_port.in_waiting:
          print(arduino_interface.serial_port.readline())
        camera.get_frame(pixel_data)
        frame_time = time.time()
        print('Copying frame took {}'.format(frame_time - start_time))
        driver.update(pixel_data)
        print('Driver update took {} seconds'.format(time.time() - frame_time))
        #Image.fromarray(pixel_data, 'RGB').show()
      #time.sleep(1.0)
  finally:
    print('Turning off motors')
    arduino_interface.command_motor_pwms(0, 0)
