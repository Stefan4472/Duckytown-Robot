'''
A multi-threaded camera interface using PiCamera.
Supports continuous video capture. Access individual frames via getFrame().

Threading concept: https://www.pyimagesearch.com/2015/12/21/increasing-webcam-fps-with-python-and-opencv/
Image capture concept: https://www.pyimagesearch.com/2015/03/30/accessing-the-raspberry-pi-camera-with-opencv-and-python/
'''
from picamera import PiCamera
from picamera.array import PiRGBArray
import threading import Thread
import time

class Camera:
  def __init__(self, resolution=(320, 240), framerate=32):
    # Create and initialize the PiCamera object.
    self.camera = PiCamera()
    self.camera.resolution = resolution
    self.camera.framerate = framerate
    # IO stream where the raw pixel data will be stored.
    # It is stored as a Numpy array with three channels (R, G, B)
    self.raw_capture = PiRGBArray(camera, size=resolution)  # TODO: THIS CAN PROBABLY BE A LOCAL VARIABLE
    # Whether the camera is currently capturing images.
    # self.running = False
    # The capture thread, which will run '_thread_capture_async()'
    self.capture_thread = Thread(target=self._thread_capture_async, args=())
    # Sleep while the camera initializes
    time.sleep(0.1)

    self.last_frame = None

    # Start!
    self.capture_thread.start()    

  # # TODO: SUPPORT START/STOP
  # # THIS FUNCTION SHOULD CHECK IF IT'S ALREADY RUNNING
  # def start_capture(self):
  #   self.running = True
  #   self.capture_thread.start()
  #   return 

  # def stop_capture(self):
  #   self.running = False 
  #   self.capture_thread.stop()
  #   return

  # Gets the most recent frame, copying it to the provided 
  # numpy array. The array must be the proper resolution!
  # TODO: USE MUTEX?
  def get_frame(self, buffer):
    np.copy_to(buffer, self.last_frame)

  # The thread's function. Must have an infinite loop.
  def _thread_capture_async(self):
    # Reference:
    # https://picamera.readthedocs.io/en/release-1.10/api_camera.html#picamera.camera.PiCamera.capture_continuous
    # https://picamera.readthedocs.io/en/release-1.10/api_array.html#pirgbarray
    self.camera.start_preview()
    
    # Infinite capture loop.
    for frame in self.camera.capture_continuous(self.raw_capture, format="rgb", use_video_port=True):
      # Copy image data to 'last_frame'
      np.copy_to(self.last_frame, frame.array)
      # Clear the stream in preparation for the next frame.
      output.truncate(0)
      # TODO: FIRE FRAME_READY() LISTENER

    # camera_on = False
    # camera_generator = None

    # while True:
    #   if self.running and not camera_on:
    #     self.camera.start_preview()
    #     camera_generator = self.camera.capture_continuous(self.raw_capture, format="rgb", use_video_port=True)
    #     camera_on = True
    #   elif not self.running and camera_on:
    #     self.camera.stop_preview()
    #     camera_generator = None
    #     camera_on = False
    #   elif self.running and camera_on:
    #     # Grab the raw NumPy array representing the image
    #     image = frame.array
    #     # Clear the stream in preparation for the next frame
    #     self.raw_capture.truncate(0)
        