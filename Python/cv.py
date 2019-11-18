#import timeit
#test = """
import numpy as np
from PIL import Image
from enum import Enum

# Colors: (R, G, B, Tolerance)
yellow = (254, 240, 82, 35)
red = (255, 73, 109, 30)
white = (254, 255, 253, 40)

### SCALED FOR 640*480 IMAGE SIZE, RESCALE FOR FINAL
img_width = 320#640
img_height = 240#480

sample_size = 4
pixels_per_cm = 12
LANE_WIDTH_PX = 240
yellow_width = 0#22
white_width = 0#32
start_row = int(img_height*(160/240))
rows_checked = int(img_height*(40/240))
start_col = 0
cols_checked = img_width


def get_position(pixel_row, pixel_col):
    return [20.0, (img_width/2.0 - pixel_col) * 1.0 / pixels_per_cm]


# Takes a pixel (red, green, blue).
# Attempts to classify the color. Returns 'Color' value.
# def getColor(pixel):
def isColor(pixel, color):
    if abs(pixel[0] - color[0]) < color[3] and \
       abs(pixel[1] - color[1]) < color[3] and \
       abs(pixel[2] - color[2]) < color[3]:
        return True
    return False

# Given an image, return its yellow, white, and red feature points (rightmost yellow, leftmost yellow, center red)
def analyze_img(m):
    foundW = False
    pYellow = []
    pWhite = []
    pRed = []
    rLeft = []
    rRight = []

    for i in range(start_row, start_row+rows_checked-1, 4):
        for j in range(start_col+cols_checked-4, start_col-1, -4):
            if not pYellow and isColor(m[i][j], yellow):
                pYellow = [i, j]    
            if not pWhite and isColor(m[i][j], white) and not foundW:
                foundW = True
            if foundW and not isColor(m[i][j], white):
                pWhite = [i, j]
                foundW = False
                return pYellow, pWhite, pRed
            if rRight and not rLeft and not isColor(m[i][j], red):
                rLeft = [i, j]
                pRed = [int((rRight[0]+rLeft[0])/2), int((rRight[1]+rLeft[1])/2)]
            if not rRight and isColor(m[i][j], red):
                rRight = [i, j]
            if pYellow and pWhite:
                break
    print(pYellow, ':', pWhite, ':', rLeft, rRight)
    return pYellow, pWhite, pRed

def inferCenter(fYellow, fWhite):
    if fYellow and fWhite:
        return (fWhite[1]+fYellow[1])/2
    elif fWhite and not fYellow:
        return fWhite[1] - LANE_WIDTH_PX/2
    elif fYellow and not fWhite:
        return fYellow[1] + LANE_WIDTH_PX/2
    else:
        return None

if __name__ == '__main__':
  # load image for testing

  im = Image.open('640p2.jpg')
  m = np.array(im)
  yc, wc, rc = analyze_img(m)
  print(yc, wc, rc)

  ctr = inferCenter(yc, wc)
  if ctr:
      rTarget = get_position(ctr)
      print("rw target:", rTarget)
  else:
      print("no features found")
      pass

  clr = (0, 0, 0)
  if yc:
    m[yc[0]][yc[1]] = clr
    m[yc[0]][yc[1]+2] = clr
    m[yc[0]+2][yc[1]] = clr
    m[yc[0]+2][yc[1]+2] = clr
  if wc:
    m[wc[0]][wc[1]] = clr
    m[wc[0]][wc[1]+2] = clr
    m[wc[0]+2][wc[1]] = clr
    m[wc[0]+2][wc[1]+2] = clr
  if rc:
    m[rc[0]][rc[1]] = clr
    m[rc[0]][rc[1]+2] = clr
    m[rc[0]+2][rc[1]] = clr
    m[rc[0]+2][rc[1]+2] = clr
  img = Image.fromarray(m, 'RGB')
  img.show()

#"""  
#elapsed_time = timeit.timeit(test, number=100)/100
#print(elapsed_time)
