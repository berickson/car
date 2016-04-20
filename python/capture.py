#!/usr/bin/env python2.7
import numpy as np
import cv2

seconds = 30
fps = 60.0
width = 640
height = 480
size = (width, height)
cam_nums = [0]
caps = [cv2.VideoCapture(n) for n in cam_nums]

for cap in caps:
  cap.set(cv2.CAP_PROP_FRAME_WIDTH,width)
  cap.set(cv2.CAP_PROP_FRAME_HEIGHT,height)
  cap.set(cv2.CAP_PROP_FPS,fps)

# Define the codec and create VideoWriter object
#fourcc = cv2.cv.CV_FOURCC('M','P','4','V'); # doesn't work on pi
#fourcc = cv2.cv.CV_FOURCC('X','V','I','D'); # doesn't work on pi
#fourcc = cv2.cv.CV_FOURCC('M','P','E','G'); # encodes in pi but doesn't open in vlc ubuntu

# timings for 100 , including capture, one eye of minoru
#         640x480   320x240   
#--------+---------+---------+
# x264    32.451s
# MJPG    24.222s
# XVID    24.026s   23.926s


codec = 'x264' # 32.451s 100 frames 640x480 
#codec = 'MJPG' # 24.222s
#codec = 'XVID' # 24.026s

fourcc = cv2.VideoWriter_fourcc(*codec)

outs = [cv2.VideoWriter('out'+str(n)+'.avi',fourcc,fps,size) for n in cam_nums]

frames_grabbed = 0
buffers = [cap.read()[1] for cap in caps]
#ret, buffer1 = cap1.read()
#ret, buffer2 = cap1.read()
frames_to_grab = 200
while(caps[0].isOpened() and frames_grabbed < frames_to_grab):
    for cap in caps: cap.grab()
    frames = list()
    for n in cam_nums:
      ret,frame = caps[n].retrieve(buffers[n]) 
      if ret==False:
          break
      outs[n].write(buffers[n])
    frames_grabbed += 1

# Release everything if job is finished

for cap in caps: cap.release()
for out in outs: out.release()

