import numpy as np
from PIL import Image

# load image for testing
im = Image.open('640p2.jpg')

m = np.array(im)

yellow = (254, 240, 82)
red = (255, 73, 109)
white = (254, 255, 253)

yellow_tolerance = 25
red_tolerance = 30
white_tolerance = 12

start_row = 220
rows_checked = 60
start_col = 0
cols_checked = 640

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

# m = m[start_row : start_row + rows_checked][start_col : start_col + cols_checked]

img = Image.fromarray(m, 'RGB')
img.show()


# y_roi = m[220:280, :240, :]
# w_roi = m[220:280, 240:, :]
# r_roi = m[220:280, :, :]
#
# print(r_roi)
#
# img = Image.fromarray(r_roi, 'RGB')
# img.show()




