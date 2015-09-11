import cv2
import numpy as np
#sudo apt-get install python-matplotlib
from matplotlib import pyplot as plt



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

ports = [1,2]
cams = []

for port in ports:
    cam = cv2.VideoCapture(port)

while(cv2.waitKey(1000)==-1):
    images = []
    for port in ports:
        retval,im = cam.read()
        if retval == False:
            print 'Image read failed'
        images.append(im)
        cv2.imshow('image'+str(port),im)
    #stereo = cv2.StereoBM(cv2.STEREO_BM_BASIC_PRESET,16,15)
    stereo = cv2.StereoBM(cv2.STEREO_BM_BASIC_PRESET,32,15)

    greyL = cv2.cvtColor(images[0], cv2.COLOR_BGR2GRAY)
    greyR = cv2.cvtColor(images[1], cv2.COLOR_BGR2GRAY)
    cv2.imshow('greyL', greyL)
    cv2.imshow('greyR', greyL)

    disparity = stereo.compute(greyR,greyL)
    cv2.imshow('disparity', disparity)
    cv2.imshow('equalized disparity', histeq(disparity))

#x=cv2.waitKey(0)
cv2.destroyAllWindows()
