import time
import numpy as np
from arduino_interface import ArduinoInterface
from driver import Driver, TurnType
from picam_interface import Camera

IMAGE_WIDTH = 240
IMAGE_HEIGHT = 320

if __name__ == '__main__':
  arduino_interface = ArduinoInterface('/dev/ttyACM0', 9600, timeout=1.0)
  camera = Camera()
  driver = Driver(arduino_interface)
  # navigator = Navigator()

  time.sleep(2)
  driver.set_speed_limit(10.0)
  driver.instruct(TurnType.STRAIGHT)
  pixel_data = np.empty(shape=(IMAGE_HEIGHT, IMAGE_WIDTH, 3), dtype='uint8')

  while True:
    if camera.frame_ready:
      print('Got next frame')
      camera.get_frame(pixel_data)
      driver.update(pixel_data)