import time
import picamera

with picamera.PiCamera() as camera:
    camera.resolution = (320, 240)
    #camera.start_preview()
    # Camera warm-up time
    time.sleep(2)
    camera.capture('photo5.jpg')
