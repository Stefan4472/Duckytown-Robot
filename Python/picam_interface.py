'''
A multi-threaded camera interface using PiCamera.
Supports continuous video capture. Access individual frames via getFrame().

Threading concept: https://www.pyimagesearch.com/2015/12/21/increasing-webcam-fps-with-python-and-opencv/
Image capture concept: https://www.pyimagesearch.com/2015/03/30/accessing-the-raspberry-pi-camera-with-opencv-and-python/
'''
from picamera import PiCamera
from picamera.array import PiRGBArray
from threading import Thread
import time
import numpy as np

# TODO: THIS CLASS NEEDS CLEANUP

class Camera:
  def __init__(self, resolution=(320, 240), framerate=32):
    # Create and initialize the PiCamera object.
    self.camera = PiCamera()
    self.camera.resolution = resolution
    self.camera.framerate = framerate
    self.frame_ready = False
    self._last_frame = np.empty((resolution[1], resolution[0], 3), dtype='uint8')
    # Whether the camera is currently capturing images.
    self.running = False
    # The capture thread, which will run '_thread_capture_async()'.
    # The thread is created when 'start()' is called.
    self.capture_thread = None

    # Diagnostics
    self.frames_captured = 0
    self.start_time = None
    # # Start!
    # self.capture_thread.start()    

  # Starts continuous capture. Creates and starts the capture thread.
  def start(self):
    if not self.running:
      self.capture_thread = Thread(target=self._thread_capture_async, args=())
      self.capture_thread.start()
      self.running = True
    return 

  # Stops continuous capture. Stops and destroys the capture thread.
  def stop(self):
    if self.running:
      self.running = False 
      self.capture_thread.stop()
      self.capture_thread = None
    return

  # Gets the most recent frame, copying it to the provided 
  # numpy array. The array must be the proper resolution!
  # TODO: USE MUTEX?
  def get_frame(self, buffer):
    np.copyto(buffer, self._last_frame)
    self.frame_ready = False

  # The thread's function. Must have an infinite loop.
  def _thread_capture_async(self):
    # Reference:
    # https://picamera.readthedocs.io/en/release-1.10/api_camera.html#picamera.camera.PiCamera.capture_continuous
    # https://picamera.readthedocs.io/en/release-1.10/api_array.html#pirgbarray
    # self.camera.start_preview()
    print('Starting capture thread')
    # IO stream where the raw pixel data will be stored.
    # It is stored as a Numpy array with three channels (R, G, B)
    raw_capture = PiRGBArray(self.camera, size=self.camera.resolution)
    self.start_time = time.time()
    # Infinite capture loop.
    for frame in self.camera.capture_continuous(raw_capture, format="rgb", use_video_port=True):
      # Copy image data to 'last_frame'
      np.copyto(self._last_frame, frame.array)
      # Clear the stream in preparation for the next frame.
      raw_capture.truncate(0)
      self.frames_captured += 1
      self.frame_ready = True

      if self.frames_captured >= 100:
        print('FPS over the last {} captures was {}'.format(\
            self.frames_captured, self.frames_captured / (time.time() - self.start_time)))
        self.frames_captured = 0
        self.start_time = time.time()

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
        
