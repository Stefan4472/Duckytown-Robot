import sys
import time
import numpy as np
from PIL import Image
from enum import Enum
'''
Image processing functions. Assumes camera resolution of 320*240px (320 wide, 240 tall).
'''

CAMERA_RESOLUTION = (320, 240)

YELLOW_RGB = (254, 240, 82)
RED_RGB = (255, 73, 109)
WHITE_RGB = (254, 255, 253)

YELLOW_TOLERANCE = 55
RED_TOLERANCE = 30
WHITE_TOLERANCE = 40

# Defines a rectangular region of interest on an image.
class RegionOfInterest:
  def __init__(self, row, col, width, height):
    self.row = row
    self.col = col
    self.width = width
    self.height = height
  
  def __repr__(self):
    return 'RegionOfInterest({}, {}, {}, {})'.format(self.row, self.col, self.width, self.height)

# Fills in the specified RegionOfInterest with the given rgb color (r, g, b).
def draw_region(image, region, rgb, line_thickness=3):
  # Top line
  for i in range(region.row, region.row + line_thickness):
    for j in range(region.col, region.col + region.width):
      image[i][j] = rgb
  # Bottom line
  for i in range(region.row + region.height - line_thickness, region.row + region.height):
    for j in range(region.col, region.col + region.width):
      image[i][j] = rgb
  # Left line
  for j in range(region.col, region.col + line_thickness):
    for i in range(region.row, region.row + region.height):
      image[i][j] = rgb
  # Right line
  for j in range(region.col + region.width - line_thickness, region.col + region.width):
    for i in range(region.row, region.row + region.height):
      image[i][j] = rgb

# Draws square of given rgb color and width at i, j.
def draw_square(img, row, col, rgb, width):
  half_width = int(width / 2.0)
  for i in range(row - half_width, row + half_width):
    for j in range(col - half_width, col + half_width):
      img[i][j] = rgb

# Define default regions of interest.
# Each color has a default region and a backup region.
# There is also a catastrophic region, to be used if 
# *no* lane markings can be found.
yellow_roi = RegionOfInterest(100, 7, 40, 40)
yellow_backup_roi = RegionOfInterest(70, 0, 65, 100)
white_roi = RegionOfInterest(100, 275, 40, 40)
white_backup_roi = RegionOfInterest(70, 255, 65, 100)
red_roi = RegionOfInterest(100, 150, 40, 40)
red_backup_roi = RegionOfInterest(130, 150, 45, 100)
catastrophic_roi = RegionOfInterest(0, 0, CAMERA_RESOLUTION[0], CAMERA_RESOLUTION[1])

# Possible colors
class Color(Enum):
  UNRECOGNIZED = 0
  YELLOW = 1
  WHITE = 2
  RED = 3
  # GREEN = 4

def get_color_params(color):
  if color == Color.YELLOW:
    return YELLOW_RGB, YELLOW_TOLERANCE
  elif color == Color.RED:
    return RED_RGB, RED_TOLERANCE
  elif color == Color.WHITE:
    return WHITE_RGB, WHITE_TOLERANCE
  else:
    raise ValueError('Invalid color')

def classify_color(pixel):
  if is_color(pixel, YELLOW_RGB, YELLOW_TOLERANCE):
    return Color.YELLOW
  elif is_color(pixel, WHITE_RGB, WHITE_TOLERANCE):
    return Color.WHITE
  elif is_color(pixel, RED_RGB, RED_TOLERANCE):
    return Color.RED
  else:
    return Color.UNRECOGNIZED
    
# Takes a pixel (red, green, blue).
# Attempts to classify the color. Returns 'Color' value.
# def getColor(pixel):
def is_color(pixel, rgb, tolerance):
  # print('Checking color on rgb, tolerance {} {}'.format(rgb, tolerance))
  return True if \
    (abs(pixel[0] - rgb[0]) < tolerance and \
     abs(pixel[1] - rgb[1]) < tolerance and \
     abs(pixel[2] - rgb[2]) < tolerance) else False

# Searches given RegionOfInterest for a box of the given color.
# Returns (row, col) of pixel if found, otherwise None. 
def search_region(image, region, color, box_size=3, step_rows=8, step_cols=8):
  # print('Searching region {} for color {}'.format(region, color))
  rgb, tolerance = get_color_params(color)
  # print('Params are {}, {}'.format(rgb, tolerance))
  for i in range(region.row, region.row + region.height, step_rows):
    for j in range(region.col, region.col + region.width, step_cols):
      # print('Checking pixel {}, {}, color={}'.format(i, j, image[i][j]))
      # if is_color(image[i][j], rgb, tolerance):
        # print('Found color at {}, {}'.format(i, j))
      if is_color(image[i][j], rgb, tolerance) and \
         check_rgb_box(image, i, j, rgb, tolerance, box_size):
        return (i, j)
  return None

# TODO: THIS MAY HAVE BOUNDING ISSUES IF CALLED ON A PIXEL RIGHT AT THE EDGE OF THE IMAGE
def check_rgb_box(image, row, col, rgb, tolerance, box_size):
  half_width = int(box_size / 2)
  return is_color(image[row - half_width][col - half_width], rgb, tolerance) and \
         is_color(image[row - half_width][col + half_width], rgb, tolerance) and \
         is_color(image[row + half_width][col - half_width], rgb, tolerance) and \
         is_color(image[row + half_width][col + half_width], rgb, tolerance)

# Searches horizontally to find the extent of pixels of the given color horizontally.
# Returns (start_point, end_point)
def get_color_extent_hz(image, row, col):
  rgb, tolerance = get_color_params(classify_color(image[row][col]))
  # print('Getting color extent on {}, {}: RGB is {}, color is {}'.format(row, col, image[row][col], classify_color(image[row][col])))
  start_col = col
  end_col = col
  
  # Scan to the right.
  j = col + 1
  while j < image.shape[1] and is_color(image[row][j], rgb, tolerance):
    end_col += 1
    j += 1
  
  # Scan to the left.
  j = col - 1
  while j > -1 and is_color(image[row][j], rgb, tolerance):
    start_col -= 1
    j -= 1

  return (row, start_col), (row, end_col)

# Searches vertically to find the extent of pixels of the given color.
# Returns (start_point, end_point)
def get_color_extent_vt(image, row, col):
  rgb, tolerance = get_color_params(classify_color(image[row][col]))
  start_row = row
  end_row = row
  
  # Scan down.
  i = row + 1
  while i < image.shape[0] and is_color(image[i][col], rgb, tolerance):
    end_row += 1
    i += 1
  
  # Scan to the left.
  i = row - 1
  while i > -1 and is_color(image[i][col], rgb, tolerance):
    start_row -= 1
    i -= 1

  return (start_row, col), (end_row, col)
  
# Scaled for 320*240  # TODO: THESE CONSTANTS WON'T HOLD FOR ALL IMAGE DEPTHS
PX_PER_CM = 12
YELLOW_LINE_WIDTH_PX = 22
WHITE_LINE_WIDTH_PX = 25
LANE_WIDTH_PX = 240

def analyze_img(img):
  yellow_center, white_center, red_center = None, None, None
  # Search default regions.
  yellow_px = search_region(img, yellow_roi, Color.YELLOW)
  white_px = search_region(img, white_roi, Color.WHITE)
  red_px = search_region(img, red_roi, Color.RED)
  # Search backup regions as needed. TODO: DO WE NEED BOTH COLORS? SHOULD WE ONLY SEARCH BACKUPS IF NEITHER COLOR SHOWS ON FIRST RUN?
  # TODO: SHOULD WE SEARCH STOP BACKUP?
  if not yellow_px:
    print('Searching yellow backup')
    yellow_px = search_region(img, yellow_backup_roi, Color.YELLOW)
  if not white_px:
    print('Searching white backup')
    white_px = search_region(img, white_backup_roi, Color.WHITE)
  # Run catastrophic region if still no results.
  if not yellow_px and not white_px:
    print('Searching catastrophic')
    yellow_px = search_region(img, catastrophic_roi, Color.YELLOW)
    white_px = search_region(img, catastrophic_roi, Color.WHITE)
  # Get lane centers.
  if yellow_px:
    start_yellow, end_yellow = get_color_extent_hz(img, yellow_px[0], yellow_px[1])
    yellow_center = yellow_px[0], start_yellow[1] + int((end_yellow[1] - start_yellow[1]) / 2.0)
  if white_px: 
    # print('Getting color extent on white at {}, {} with rgb {}'.format(white_px[0], white_px[1], img[white_px[0], white_px[1]]))
    start_white, end_white = get_color_extent_hz(img, white_px[0], white_px[1])
    white_center = white_px[0], start_white[1] + int((end_white[1] - start_white[1]) / 2.0)
  if red_px:
    start_red, end_red = get_color_extent_vt(img, red_px[0], red_px[1])
    red_center = start_red[0] + int((end_red[0] - start_red[0]) / 2.0), red_px[1]
  return yellow_center, white_center, red_center


if __name__ == '__main__':
  if len(sys.argv) < 2:
    print('Missing argument: path to image')
    sys.exit(1)
  # load image for testing
  img_path = sys.argv[1]
  print('Analyzing image {}'.format(img_path))
  img = np.array(Image.open(img_path).resize(CAMERA_RESOLUTION))

  start_time = time.time()
  y, w, r = analyze_img(img)
  end_time = time.time()
  print('Got Yellow: {}'.format(y))
  print('Got White: {}'.format(w))
  print('Got Red: {}'.format(r))
  print('Took {} seconds'.format(end_time - start_time))

  draw_region(img, yellow_roi, (255, 0, 0))
  draw_region(img, yellow_backup_roi, (200, 0, 0))
  draw_region(img, white_roi, (0, 255, 0))
  draw_region(img, white_backup_roi, (0, 200, 0))
  draw_region(img, red_roi, (0, 0, 255))
  draw_region(img, red_backup_roi, (0, 0, 200))

  if y:
    draw_square(img, y[0], y[1], (0, 0, 0), 5)
  if w:
    draw_square(img, w[0], w[1], (0, 0, 0), 5)
  if r:
    draw_square(img, r[0], r[1], (0, 0, 0), 5)

  Image.fromarray(img, 'RGB').show()
  
