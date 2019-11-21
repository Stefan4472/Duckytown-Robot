import time
import numpy as np
from PIL import Image, ImageTk
# import pygame
from picam_interface import Camera
import tkinter as tk
from picamera import PiCamera
from picamera.array import PiRGBArray

IMAGE_RESOLUTION = (320, 240)		
pixel_data = np.zeros(shape=(IMAGE_RESOLUTION[1], IMAGE_RESOLUTION[0], 3), dtype='uint8')

def update_display():   
    print('Updating display')
    next(camera_generator)
    next_image = ImageTk.PhotoImage(Image.fromarray(raw_capture.array, 'RGB'))
    canvas.itemconfig(image_on_canvas, image=next_image)
    # Clear the stream in preparation for the next frame.
    raw_capture.truncate(0)
    root.after(1000, update_display)
    

    
camera = PiCamera()
camera.resolution = (320, 240)
camera.framerate = 30
raw_capture = PiRGBArray(camera, size=camera.resolution)
next_image = None 
#camera_generator = camera.capture_continuous(raw_capture, format="rgb", use_video_port=True)

root = tk.Tk()
canvas = tk.Canvas(root, width=320, height=240)
canvas.grid(row=0, column=0)
image_on_canvas = canvas.create_image(0, 0, anchor=tk.NW)
update_display()
root.mainloop()
      
'''
while True:
    print('frame_ready = {}'.format(camera.frame_ready))
    if camera.frame_ready:
        print('Got a new frame')
        camera.get_frame(pixel_data)
        next_image = Image.fromarray(pixel_data, 'RGB')
        canvas.itemconfig(image_on_canvas, image=next_image)
        #time.sleep(2)
'''
