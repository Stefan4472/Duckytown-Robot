from arduino_interface import ArduinoInterface
from driver import Driver, TurnType
from picam_interface import Camera
import time 

arduino_interface = ArduinoInterface('/dev/ttyACM0', 9600, timeout=1.0)
camera = Camera()
driver = Driver()
# navigator = Navigator()

time.sleep(2)
driver.instruct(TurnType.STRAIGHT)
while True:
  if camera.frame_ready:
    ... get_frame()
    ... driver.update(next_img)