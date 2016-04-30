#!/usr/bin/env python2.7

# calibrate camera based on input images
# based on example from
# http://docs.opencv.org/3.0-beta/doc/py_tutorials/py_calib3d/py_calibration/py_calibration.html#

import numpy as np
import cv2
import glob
import threading
import time

import os

show_images = True

def filename_from_path(path):
    basename = os.path.basename(path)
    filename = basename.split('.')[0]
    return filename

def get_yes_no(s):
    while True:
      x = raw_input(str(s)+str(' (y/n)'))
      if x == 'y':
          return True
      if x == 'n':
          return False
      print 'please enter y/n'


class FrameGrabber:
  lock = threading.Lock()
  def __init__(self,camera):
    self._quit = False
    self.camera = camera
    self.img = None
    
  def _grab_thread(self):
    while self._quit == False:
      FrameGrabber.lock.acquire()
      try:
        self.camera.grab()
      finally:
        FrameGrabber.lock.release()
      time.sleep(0.001)
    
  def go(self):
    self._thread = threading.Thread(target = self._grab_thread)
    self._thread.start()
  
  def quit(self):
    self._quit = True
    self._thread.join()
    
  def read(self):
    FrameGrabber.lock.acquire()
    try:
      if self.img is None:
        retval,img = self.camera.retrieve()
      else:
        retval,img = self.camera.retrieve(self.img)
      if retval: self.img = img
    finally:
      FrameGrabber.lock.release()
    return (retval,img)

def grab_images():
  grabbers = []
  for i in range(2):
    try:
      camera = cv2.VideoCapture(i)
      camera.set(cv2.CAP_PROP_FRAME_WIDTH,320)
      camera.set(cv2.CAP_PROP_FRAME_HEIGHT,240)
      camera.set(cv2.CAP_PROP_FPS,60)
      grabber = FrameGrabber(camera)
      grabbers.append(grabber)
      grabber.go()
    except:
      raise
      
  print 'found',str(len(grabbers)),'cameras'
  
  while True:
    c = cv2.waitKey(1)
    if c <> -1:
      print 'pressed: '+str(c)
    c = c & 255
    if c == ord('q'):
      break
    images = []
    for grabber in grabbers:
      retval,img = grabber.read()
      if retval: images.append(img)
        
    for i in range(len(images)):
      cv2.imshow(str(i),images[i])
    if c == ord(' '):
      print 'saving images'
      for i in range(len(images)):
        cv2.imwrite('img_'+str(i)+'.png',images[i])
   
  for grabber in grabbers:
    grabber.quit()
    

if get_yes_no('do you want to grab images?'):
    grab_images()
    exit()


# termination criteria
criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 30, 0.001)

# prepare object points, like (0,0,0), (1,0,0), (2,0,0) ....,(6,5,0)
objp = np.zeros((6*7,3), np.float32)
objp[:,:2] = np.mgrid[0:7,0:6].T.reshape(-1,2)

# Arrays to store object points and image points from all the images.
objpoints = [] # 3d point in real world space
imgpoints = [] # 2d points in image plane.

images = glob.glob('data/*.jpg')

for fname in images:
    img = cv2.imread(fname)
    gray = cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)

    # Find the chess board corners
    ret, corners = cv2.findChessboardCorners(gray, (7,6),None)

    # If found, add object points, image points (after refining them)
    if ret == True:
        objpoints.append(objp)

        cv2.cornerSubPix(gray,corners,(11,11),(-1,-1),criteria)
        imgpoints.append(corners)
        if show_images:
            # Draw and display the corners
            cv2.drawChessboardCorners(img, (7,6), corners,ret)
            cv2.imshow('img',img)
            cv2.waitKey(500)

cv2.destroyAllWindows()


###################################
# Calculate Calibration Parmeters

ret, mtx, dist, rvecs, tvecs = cv2.calibrateCamera(
    objpoints, imgpoints, gray.shape[::-1],None,None)

img = cv2.imread('data/left12.jpg')
h,  w = img.shape[:2]
newcameramtx, roi=cv2.getOptimalNewCameraMatrix(mtx,dist,(w,h),1,(w,h))


##################################
# Re-project corners to test paramters
print imgpoints[0].size
tot_error = 0.
tot_obj_error = 0.
for i in xrange(len(objpoints)):
    imgpoints2, _ = cv2.projectPoints(objpoints[i], rvecs[i], tvecs[i], mtx, dist)
    print imgpoints2.size
    #tot_obj_error += cv2.norm(objpoints[i],imgpoints2, cv2.NORM_L2)
    tot_error +=  cv2.norm(imgpoints[i],imgpoints2, cv2.NORM_L2)

print "original error: ", tot_obj_error/len(objpoints)
print "corrected error: ", tot_error/len(objpoints)


print "undistoring images"
###################################
# Medhod 1 - Undistort
for fname in images:
    img = cv2.imread(fname)
    dst = cv2.undistort(img, mtx, dist, None, newcameramtx)

    # crop the image
    x,y,w,h = roi
    dst = dst[y:y+h, x:x+w]
    outfile = 'output/undistor-{0}.jpg'.format(filename_from_path(fname))
    cv2.imwrite(outfile,dst)

print "remaping images"
###################################
# Medhod 2 - remap
for fname in images:
    img = cv2.imread(fname)
    mapx,mapy = cv2.initUndistortRectifyMap(mtx,dist,None,newcameramtx,(w,h),5)
    dst = cv2.remap(img,mapx,mapy,cv2.INTER_LINEAR)

    # crop the image
    x,y,w,h = roi
    dst = dst[y:y+h, x:x+w]
    outfile = 'output/remap-{0}.jpg'.format(filename_from_path(fname))
    cv2.imwrite(outfile,dst)

print "done."
