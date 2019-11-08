import time
import picamera
import numpy as np
from PIL import Image

def process_img(m):
	yellow = (254, 240, 82)
	red = (255, 73, 109)
	white = (254, 255, 253)

	yellow_tolerance = 25
	red_tolerance = 30
	white_tolerance = 12

	start_row = 0
	rows_checked = 240
	start_col = 0
	cols_checked = 320

	for i in range(start_row, start_row + rows_checked):
		for j in range(start_col, start_col + cols_checked):
			if abs(m[i][j][0] - yellow[0]) < yellow_tolerance and \
			   abs(m[i][j][1] - yellow[1]) < yellow_tolerance and \
			   abs(m[i][j][2] - yellow[2]) < yellow_tolerance:
				m[i][j] = [255, 0, 0]
			elif abs(m[i][j][0] - red[0]) < red_tolerance and \
				 abs(m[i][j][1] - red[1]) < red_tolerance and \
				 abs(m[i][j][2] - red[2]) < red_tolerance:
				m[i][j] = [0, 255, 0]
			elif abs(m[i][j][0] - white[0]) < white_tolerance and \
				 abs(m[i][j][1] - white[1]) < white_tolerance and \
				 abs(m[i][j][2] - white[2]) < white_tolerance:
				m[i][j] = [0, 0, 255]
            
if __name__ == '__main__':
	with picamera.PiCamera() as camera:
		camera.resolution = (320, 240)
		camera.framerate = 24
		time.sleep(2)
		cam_output = np.empty((240, 320, 3), dtype=np.uint8)
		
		camera.capture(cam_output, 'rgb')
		
		# WE CAN'T PREVIEW OVER TEAMVIEWER
		#camera.start_preview()
		#time.sleep(5)
		#camera.stop_preview()

	process_img(cam_output)
	img = Image.fromarray(cam_output, 'RGB')
	img.save('output.png')
