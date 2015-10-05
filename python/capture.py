#!/usr/bin/env python2.7
import numpy as np
import cv2

cap = cv2.VideoCapture(0)
print 1
# Define the codec and create VideoWriter object
#fourcc = cv2.cv.CV_FOURCC('M','P','4','V'); # doesn't work on pi
#fourcc = cv2.cv.CV_FOURCC('X','V','I','D'); # doesn't work on pi
#fourcc = cv2.cv.CV_FOURCC('M','P','E','G'); # encodes in pi but doesn't open in vlc ubuntu
fourcc = cv2.VideoWriter_fourcc(*'XVID')#cv2.cv.CV_FOURCC('x','2','6','4');

fps = 20.0
size = (640,480)
out = cv2.VideoWriter('output.avi',fourcc, fps, size)
print 2
seconds = 5
frames_grabbed = 0
while(cap.isOpened() and frames_grabbed < seconds*fps):
    print '.',
    ret, frame = cap.read()
    frames_grabbed += 1
    if ret==False:
        break
    out.write(frame)

# Release everything if job is finished
cap.release()
out.release()

