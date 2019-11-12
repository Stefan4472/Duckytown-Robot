import time
import numpy as np
from PIL import Image, ImageTk
# import pygame
# from picam_interface import Camera
import tkinter as tk

# TODO: CAPTURE A SINGLE IMAGE AND DISPLAY
def update_display():
	print('Updating display')
	
	if camera.frame_ready:
		print('Got next frame')
		 # Copy frame to 'pixel_data' buffer
		camera.get_frame(pixel_data)
		# Create Tkinter image from numpy image frame 
		tk_image = ImageTk.PhotoImage(image=Image.fromarray(pixel_data))
		# Update the canvas image
		tk_canvas.itemconfig(tk_canvas_img, image=tk_image)
	
	# Call 'update_display()' every x milliseconds
	tk_root.after(1000, update_display)

IMAGE_RESOLUTION = (320, 240)	# TODO: MAKE RES_W, RES_H INSTEAD		
# camera = Camera()
pixel_data = np.zeros(shape=(IMAGE_RESOLUTION[1], IMAGE_RESOLUTION[0], 3), dtype='uint8')
tk_root = tk.Tk()
tk_photo_img = ImageTk.PhotoImage(image=Image.fromarray(pixel_data))
tk_canvas = tk.Canvas(tk_root, width=IMAGE_RESOLUTION[0], height=IMAGE_RESOLUTION[1])
tk_canvas_img = tk_canvas.create_image(0, 0, anchor='nw', image=tk_photo_img)

if __name__ == '__main__':
	tk_canvas.pack()
	update_display()
	tk_root.mainloop()
	'''
	# Code to display numpy array using Pygame: https://stackoverflow.com/a/41171153
	# Also, this may be helpful: https://stackoverflow.com/a/49593410
	pygame.init()
	display = pygame.display.set_mode((IMAGE_RESOLUTION[1], IMAGE_RESOLUTION[0]))

	# TODO: HOW TO COPY 3-CHANNEL IMAGES INTO A SINGLE CHANNEL?
	img = np.zeros(shape=(IMAGE_RESOLUTION[1], IMAGE_RESOLUTION[0], 3), dtype='uint8')
	
	running = True
	
	camera = Camera()
	

	# HOW TO DRAW THE IMAGE? THIS MAY BE HELPFUL: https://github.com/amremam2004/py2cairo/blob/2f1f3788fa2d9b693c77645edc44f35e7073aadd/test/pygame-test1.py
	# Also: https://www.pygame.org/docs/ref/surfarray.html
	# Also: https://www.pygame.org/docs/tut/SurfarrayIntro.html
	while running:
		print('Updating display')
		for event in pygame.event.get():
			if event.type == pygame.QUIT:
					running = False
		# Get next capture and copy to the img array
		if camera.frame_ready:
			print('Main sees that a frame is ready')
			camera.get_frame(img)
			process_img(img)
		# Draw to the screen.
		pixel_array = pygame.PixelArray(display)
		# pixel_array[:, :] = (255, 255, 255)

		# Copy the image into the surface (slow :(... Need to merge three channels into one)	
		for i in range(IMAGE_RESOLUTION[1]):
			for j in range(IMAGE_RESOLUTION[0]):
				pixel_array[i, j] = (img[i, j, 0], img[i, j, 1], img[i, j, 2])
		# print (pixel_array.shape)
		# display.blit(pixel_array.surface, (0, 0))
		# Update screen.
		pygame.display.update()

	pygame.quit()
	'''
