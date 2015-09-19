#!/usr/bin/env python2.7

import cv2
import numpy as np
import time



def histeq(im,nbr_bins=2**16):
   '''histogram equalization'''
   #get image histogram
   imhist,bins = np.histogram(im.flatten(),nbr_bins,normed=True)
   imhist[0] = 0

   cdf = imhist.cumsum() #cumulative distribution function
   cdf ** .5
   cdf = (2**16-1) * cdf / cdf[-1] #normalize
   #cdf = cdf / (2**16.)  #normalize

   #use linear interpolation of cdf to find new pixel values
   im2 = np.interp(im.flatten(),bins[:-1],cdf)

   return np.array(im2, int).reshape(im.shape)

ports = [0,1]
cams = []

for port in ports:
    cap = cv2.VideoCapture(port)
    cap.set(cv2.cv.CV_CAP_PROP_FRAME_WIDTH,640/2)
    cap.set(cv2.cv.CV_CAP_PROP_FRAME_HEIGHT,480/2)
    cap.set(cv2.cv.CV_CAP_PROP_FPS,1)
    cams.append(cap)
frame_count=0
while(cv2.waitKey(1)==-1):
    frame_count += 1
    if frame_count%10 == 0:
        print(frame_count)
    images = []
    for cam in [0,1]:
        cams[cam].grab()

    for cam in [0,1]:
        retval,im = cams[cam].retrieve()
        if retval == False:
            print 'Image read failed'
        images.append(im)
        cv2.imshow('image'+str(cam),im)
    stereo = cv2.StereoBM(cv2.STEREO_BM_BASIC_PRESET,16,15)
    stereo = cv2.StereoBM(cv2.STEREO_BM_BASIC_PRESET,64,31)

    greyL = cv2.cvtColor(images[1], cv2.COLOR_BGR2GRAY)
    greyR = cv2.cvtColor(images[0], cv2.COLOR_BGR2GRAY)
    cv2.imshow('greyL', greyL)
    cv2.imshow('greyR', greyR)

    disparity = stereo.compute(greyL,greyR)
    #cv2.imshow('disparity', disparity)
    cv2.imshow('equalized disparity', histeq(disparity))

#x=cv2.waitKey(0)
cv2.destroyAllWindows()
