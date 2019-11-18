import numpy as np
from PIL import Image
from enum import Enum

yellow = (254, 240, 82)
red = (255, 73, 109)
white = (254, 255, 253)

yellow_tolerance = 55
red_tolerance = 65
white_tolerance = 40

# # Possible colors
# class Color(Enum):
#   UNRECOGNIZED = 0
#   YELLOW = 1
#   WHITE = 2
#   RED = 3
#   # GREEN = 4

### SCALED FOR 640*480 IMAGE SIZE, RESCALE FOR FINAL
'''
sample_size = 3
pixels_per_cm = 24
center = 320
yellow_width = 45
lane_width = 477
'''
img_width = 240
img_height = 320
# Scaled for 320*240
sample_size = 4
pixels_per_cm = 12
center = 160
yellow_width = 22
white_width = 50
LANE_WIDTH_PX = 240

# Convert pixel position to a real-world robot position (cm x, cm y)
def get_position(pixel_row, pixel_col):
    return (20.0, (center - pixel_col) * 1.0 / pixels_per_cm)
    
start_row = 160
rows_checked = 40
start_col = 0
cols_checked = 320

# Takes a pixel (red, green, blue).
# Attempts to classify the color. Returns 'Color' value.
# def getColor(pixel):

def isColor(pixel, color, tolerance):
    if abs(pixel[0] - color[0]) < tolerance and \
       abs(pixel[1] - color[1]) < tolerance and \
       abs(pixel[2] - color[2]) < tolerance:
        return True
    return False

def analyze_img(m):
  # flags for if a color is found, if we found a color we stop searching for it in loop
  found_y = False
  found_w = False
  found_r = False
  right_y = False
  right_w = False
  right_r = False
  # tuples for location of colors where 0 = row of left color, 1 = col of left color,
  #                                     2 = row of right color, 3 = col of right color
  yellow_loc = [0, 0, 0, 0]
  white_loc = [0, 0, 0, 0]
  red_loc = [0, 0, 0, 0]

  #TODO: Add green processing to loop
  col_skip = 2;
  row_skip = 5;
  # loop through picture, looking for pixels that classify as red, yellow, or white
  for i in range(start_row, start_row + rows_checked, row_skip):
      for j in range(start_col, start_col + cols_checked-col_skip, col_skip):
          if (i+sample_size >= 240) or (j + sample_size >= 320):
              continue
          if not found_y and isColor(m[i][j], yellow, yellow_tolerance) and \
                          isColor(m[i + sample_size][j], yellow, yellow_tolerance) and \
                          isColor(m[i][j + sample_size], yellow, yellow_tolerance) and \
                          isColor(m[i + sample_size][j + sample_size], yellow, yellow_tolerance):
              found_y = True  # set flag to prevent searching for yellow
              # m[i][j] = [255, 0, 0]  # test code to set color of yellow pixel
              yellow_loc[0] = i  # set color location to coordinates
              yellow_loc[1] = j
          elif not found_r and isColor(m[i][j], red, red_tolerance) and \
                  isColor(m[i + sample_size][j], red, red_tolerance) and \
                  isColor(m[i][j + sample_size], red, red_tolerance) and \
                  isColor(m[i + sample_size][j + sample_size], red, red_tolerance):
              found_r = True
              # m[i][j] = [0, 255, 0]
              red_loc[0] = i
              red_loc[1] = j
          elif not found_w and isColor(m[i][j], white, white_tolerance) and \
                  isColor(m[i + sample_size][j], white, white_tolerance) and \
                  isColor(m[i][j + sample_size], white, white_tolerance) and \
                  isColor(m[i + sample_size][j + sample_size], white, white_tolerance):
              found_w = True
              # m[i][j] = [0, 0, 255]
              white_loc[0] = i
              white_loc[1] = j

  # after looping from left, loop from right on the row where we found the color to find coordinates of right side
  for j in range(start_col, start_col + cols_checked-col_skip, col_skip):
      if (i+sample_size >= 240) or (j+sample_size >= 320):
          continue
      if not right_y and isColor(m[yellow_loc[0]][cols_checked - 1 - j], yellow, yellow_tolerance) and \
              isColor(m[yellow_loc[0] + sample_size][cols_checked - 1 - j], yellow, yellow_tolerance) and \
              isColor(m[yellow_loc[0]][cols_checked - 1 - j - sample_size], yellow, yellow_tolerance) and \
              isColor(m[yellow_loc[0] + sample_size][cols_checked - 1 - j - sample_size], yellow, yellow_tolerance):
          right_y = True  # invert right_y so we stop checking for yellow
          yellow_loc[2] = yellow_loc[0]
          yellow_loc[3] = cols_checked - 1 - j  # set column for rightmost yellow pixel
      elif not right_r and isColor(m[red_loc[0]][cols_checked - 1 - j], red, red_tolerance) and \
              isColor(m[red_loc[0] + sample_size][cols_checked - 1 - j], red, red_tolerance) and \
              isColor(m[red_loc[0]][cols_checked - 1 - j - sample_size], red, red_tolerance) and \
              isColor(m[red_loc[0] + sample_size][cols_checked - 1 - j - sample_size], red, red_tolerance):
          right_r = True  # invert right_r so we stop checking for red
          red_loc[2] = red_loc[0]
          red_loc[3] = cols_checked - 1 - j  # set column for rightmost red pixel
      elif not right_w and isColor(m[white_loc[0]][cols_checked - 1 - j], white, white_tolerance) and \
              isColor(m[white_loc[0] + sample_size][cols_checked - 1 - j], white, white_tolerance) and \
              isColor(m[white_loc[0]][cols_checked - 1 - j - sample_size], white, white_tolerance) and \
              isColor(m[white_loc[0] + sample_size][cols_checked - 1 - j - sample_size], white, white_tolerance):
          right_w = True  # invert right_w so we stop checking for white
          white_loc[2] = white_loc[0]
          white_loc[3] = cols_checked - 1 - j  # set column for rightmost white pixel

  #TODO: get green center

  # all centers are just the leftmost pixel + (rightmost pixel - leftmost pixel) / 2
  green_center = [0, 0]
  yellow_center = [yellow_loc[0], int(yellow_loc[1] + (yellow_loc[3] - yellow_loc[1]) / 2)] if found_y else None
  stop_center = [red_loc[0], int(red_loc[1] + (red_loc[3] - red_loc[1]) / 2)] if found_r else None
  white_center = [white_loc[0], int(white_loc[1] + (white_loc[3] - white_loc[1]) / 2)] if found_w else None

  # Return center of yellow lane, white lane, stop line (if found).
  # Each return is a tuple of (row, col) pixel coordinates
  return yellow_center, white_center, stop_center #, green_center


if __name__ == '__main__':
  # load image for testing
  im = Image.open(r'/home/sam/Desktop/640p2.jpg')
  m = np.array(im)
  img = Image.fromarray(m, 'RGB')
  y, w, s = analyze_img(m)
  print(y)
  print(w)
  print(s)
  img.show()
