import sensor, image, time
import pyb
import math
from pyb import UART

ROBOT_A = True
YELLOW = 1
BLUE = 2
FRAME_WIDTH = 320
FRAME_HEIGHT = 240
draw = True

# Individual
thresholds = [(61, 86, -14, -5, 27, 60),(25, 40, -8, 8, -43, -21)] # Yellow  is first

# Superteam
#if ROBOT_A:
    #thresholds = [((39, 57, 5, 40, 8, 57),), ((17, 26, -2, 41, -59, -14),)] # Yellow  is first
#else:
    #thresholds = [((38, 61, 4, 43, 7, 64),), ((16, 21, -1, 23, -35, -16),)] # Yellow  is first

sensor.reset()                      # Reset and initialize the sensor.
sensor.set_pixformat(sensor.RGB565) # Set pixel format to RGB565 (or GRAYSCALE)
sensor.set_framesize(sensor.QVGA)   # Set frame size to QsVGA (320x240)
sensor.skip_frames(time = 2000)
sensor.set_auto_gain(False)
sensor.set_auto_whitebal(True)#False, (-1.72093, -6.02073, -3.45436))
sensor.set_brightness(1)
sensor.set_contrast(0)
clock = time.clock()
uart = UART(3, 9600, timeout_char = 10)

while(True):
    data = [160, 120, 160, 120]
    clock.tick()
    img = sensor.snapshot()
    blobs = img.find_blobs(thresholds, x_stride=2, y_stride = 2, area_threshold = 0, pixel_threshold = 200, merge = False, margin = 23)
    blobs = sorted(blobs, key=lambda blob: -blob.area())
    yellow = None
    blue = None
    for blob in blobs:
        if math.sqrt((blob.cx()-160)**2 + (blob.cy()-120)**2) < 100:
            if data[0] == 160 and data[1] == 120 and blob.code() == YELLOW:
                data[0] = 320 - blob.cx()
                data[1] = 240 - blob.cy()
            if data[2] == 160 and data[3] == 120 and blob.code() == BLUE:
                data[2] = 320 - blob.cx()
                data[3] = 240 - blob.cy()
    uart.writechar(255)
    uart.writechar(255)
    for byte in data:
        uart.writechar(byte)
        if draw:
            img.draw_circle(160, 120, 100)
            img.draw_line(int(round(FRAME_WIDTH)/2 - 10), int(round(FRAME_HEIGHT / 2)), int(round(FRAME_WIDTH / 2) + 10), int(round(FRAME_HEIGHT / 2)))
            img.draw_line(int(round(FRAME_WIDTH)/2), int(round(FRAME_HEIGHT / 2) + 10), int(round(FRAME_WIDTH / 2)), int(round(FRAME_HEIGHT / 2) -10))
            img.draw_line(int(round(FRAME_WIDTH)/2), int(round(FRAME_HEIGHT / 2) + 10), int(round(FRAME_WIDTH / 2)), int(round(FRAME_HEIGHT / 2) -10))
            img.draw_line(round(FRAME_WIDTH / 2), round(FRAME_HEIGHT / 2), 320 - data[2], 240 - data[3])
            img.draw_line(round(FRAME_WIDTH / 2), round(FRAME_HEIGHT / 2), 320 - data[0], 240 - data[1])


# import sensor, image, time
# import pyb
# from math import atan2, sqrt
# from pyb import UART

# ATTACK_BLUE = True
# DEGREES_TO_RADIANS = 0.017453292519943295769236907684886
# RADIANS_TO_DEGREES = 57.295779513082320876798154814105

# # Individual
# thresholds = [(61, 86, -14, -5, 27, 60),(25, 40, -8, 8, -43, -21)]

# sensor.reset()                      # Reset and initialize the sensor.
# sensor.set_pixformat(sensor.RGB565) # Set pixel format to RGB565 (or GRAYSCALE)
# sensor.set_framesize(sensor.QVGA)   # Set frame size to QsVGA (320x240)
# sensor.skip_frames(time = 2000)
# sensor.set_auto_gain(False)
# sensor.set_auto_whitebal(True)#False, (-1.72093, -6.02073, -3.45436))
# sensor.set_brightness(1)
# sensor.set_contrast(0)
# clock = time.clock()
# uart = UART(3, 9600, timeout_char = 10)

# YELLOW = 1
# BLUE = 2
# ORANGE = 3

# def scanBlobs(blobs, colour):
#     largest_blob = None
#     bbArea = 0

#     for blob in blobs:
#         if blob.code() != colour:
#             continue

#         if largest_blob == None:
#             largest_blob = blob
#             bbArea = blob.area()
#         else:
#             blobArea = blob.area()

#             if blobArea > bbArea:
#                 largest_blob = blob
#                 bbArea = blobArea

#     return largest_blob


# if ATTACK_BLUE:
#     while(True):
#         data = [-1, -1, -1, -1]
#         clock.tick()
#         img = sensor.snapshot()
#         blobs = img.find_blobs(thresholds, x_stride=5, y_stride=5, area_threshold=200, pixel_threshold=200, merge=True, margin=23)
#         if len(blobs) != 0:
#             largest_blu = scanBlobs(blobs, BLUE)
#             if largest_blu != None:
#                 data[0] = round(360 - RADIANS_TO_DEGREES * atan2(largest_blu.cy() - 120, largest_blu.cx() - 160))
#                 if data[0] > 360:
#                     data[0] -= 360
#                 if data[0] < 0:
#                     data[0] += 360
#                 data[1] = round(sqrt((largest_blu.cx() - 160)**2 + (largest_blu.cy() - 120)**2))
#                 img.draw_line(160, 120, largest_blu.cx(), largest_blu.cy()) # Not needed for Gameplay
#         #if len(blobs) != 0:
#             #largest_yel = scanBlobs(blobs, YELLOW)
#             #if largest_yel != None:
#                 #data[2] = round(360 - RADIANS_TO_DEGREES * atan2(largest_yel.cy() - 120, largest_yel.cx() - 160))
#                 #if data[2] > 360:
#                     #data[2] -= 360
#                 #if data[2] < 0:
#                     #data[2] += 360
#                 #data[3] = round(sqrt((largest_yel.cx() - 160)**2 + (largest_yel.cy() - 120)**2))
#                 #img.draw_line(160, 120, largest_yel.cx(), largest_yel.cy()) # Not needed for Gameplay
#         #print(clock.fps()) # Not needed for Gameplay
#         uart.writechar(255)
#         uart.writechar(255)
#         uart.writechar(data[0] >> 8)
#         uart.writechar(data[0])
#         uart.writechar(data[1])
#         #uart.writechar(data[2] >> 8)
#         #uart.writechar(data[2])
#         #uart.writechar(data[3])
#         #img.draw_line(82, 60, 78, 60)
#         #img.draw_line(80, 62, 80, 58)
# else:
#     while(True):
#         data = [-1, -1, -1, -1]
#         clock.tick()
#         img = sensor.snapshot()
#         blobs = img.find_blobs(thresholds, x_stride=5, y_stride=5, area_threshold=200, pixel_threshold=200, merge=True, margin=23)
#         if len(blobs) != 0:
#             largest_yel = scanBlobs(blobs, YELLOW)
#             if largest_yel != None:
#                 data[0] = round(360 - RADIANS_TO_DEGREES * atan2(largest_yel.cy() - 120, largest_yel.cx() - 160))
#                 if data[0] > 360:
#                     data[0] -= 360  
#                 if data[0] < 0:
#                     data[0] += 360
#                 data[1] = round(sqrt((largest_yel.cx() - 80)**2 + (largest_yel.cy() - 60)**2))
#                 img.draw_line(160, 120, largest_yel.cx(), largest_yel.cy()) # Not needed for Gameplay
#         # if len(blobs) != 0:
#         #     largest_blu = scanBlobs(blobs, YELLOW)
#         #     if largest_blu != None:
#         #         data[2] = round(360 - RADIANS_TO_DEGREES * atan2(largest_blu.cy() - 120, largest_blu.cx() - 160))
#         #         if data[2] > 360:
#         #             data[2] -= 360
#         #         if data[2] < 0:
#         #             data[2] += 360
#         #         data[3] = round(sqrt((largest_blu.cx() - 80)**2 + (largest_blu.cy() - 60)**2))
#                 #img.draw_line(160, 120, largest_blu.cx(), largest_blu.cy()) # Not needed for Gameplay
#         #print(clock.fps()) # Not needed for Gameplay
#         uart.writechar(255)
#         uart.writechar(255)
#         uart.writechar(data[0] >> 8)
#         uart.writechar(data[0])
#         uart.writechar(data[1])
#         # uart.writechar(data[2] >> 8)
#         # uart.writechar(data[2])
#         # uart.writechar(data[3])
#         #print(data[0], ' ', data[1])
#         #img.draw_line(82, 60, 78, 60)
#         #img.draw_line(80, 62, 80, 58)
