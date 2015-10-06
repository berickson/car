#!/usr/bin/env python2.7
import numpy as np
import cv2

seconds = 30
fps = 30.0
width = 320
height = 240
size = (width, height)


cap1 = cv2.VideoCapture(0)
cap2 = cv2.VideoCapture(1)
caps = [cap1,cap2]
for cap in caps:
  cap.set(cv2.CAP_PROP_FRAME_WIDTH,width)
  cap.set(cv2.CAP_PROP_FRAME_HEIGHT,height)
  cap.set(cv2.CAP_PROP_FPS,fps)



print 1
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
#fourcc = cv2.VideoWriter_fourcc(*'x264')# 32.451s 100 frames 640x480 
#fourcc = cv2.VideoWriter_fourcc(*'MJPG')# 24.222s
fourcc = cv2.VideoWriter_fourcc(*'XVID')# 24.026s




out1 = cv2.VideoWriter('out1.avi',fourcc, fps, size)
out2 = cv2.VideoWriter('out2.avi',fourcc, fps, size)
outs = [out1,out2]
print 2

frames_grabbed = 0
ret, buffer1 = cap1.read()
ret, buffer2 = cap1.read()
while(cap.isOpened() and frames_grabbed < seconds * fps):
    for cap in caps: cap.grab()
    ret, frame = cap1.retrieve(buffer1)
    ret, frame = cap2.retrieve(buffer2)
    frames_grabbed += 1
    if ret==False:
        break
    out1.write(buffer1)
    out2.write(buffer2)

# Release everything if job is finished

for cap in caps: cap.release()
for out in outs: out.release()

