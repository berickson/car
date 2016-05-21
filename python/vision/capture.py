#!/usr/bin/env python2.7
import numpy as np
import cv2
import threading
import time
import frame_grabber



class Capture:
  def __init__(self,stereo_video_paths = None):
    self.stereo_video_paths = stereo_video_paths
    
  def begin(self):
    self.done = False
    self.thread = threading.Thread(target=self.capture, args=())
    self.thread.start()
    
  def end(self):
    self.done = True
    self.thread.join(1.0) # wait up to 1 second to terminate
    self.thead = None
  
  
  def capture(self):
    seconds = 10
    fps = 30.0
    width = 320
    height = 240
    size = (width, height)

    self.grabbers = []
    cam_nums = [0,1]
    for i in cam_nums:
      try:
        camera = cv2.VideoCapture(i)
        camera.set(cv2.CAP_PROP_FRAME_WIDTH,320)
        camera.set(cv2.CAP_PROP_FRAME_HEIGHT,240)
        camera.set(cv2.CAP_PROP_FPS,60)
        grabber = frame_grabber.FrameGrabber(camera)
        self.grabbers.append(grabber)
        grabber.go()
      except:
        raise    
    

    codec = 'MPEG'
    fourcc = cv2.VideoWriter_fourcc(*codec)

    outs = [cv2.VideoWriter(f,fourcc,fps,size) for f in self.stereo_video_paths]
    for o in outs:
      o.set(cv2.CAP_PROP_FRAME_WIDTH,width)
      o.set(cv2.CAP_PROP_FRAME_HEIGHT,height)
      o.set(cv2.CAP_PROP_FPS,fps)

    frames_grabbed = 0
    buffers = [grabber.read()[1] for grabber in self.grabbers]
    frames_to_grab = seconds * fps
    while(not self.done):
        for n in range(len(self.grabbers)):
          time.sleep(0.01)
          grabber = self.grabbers[i]
          ret,frame = grabber.read(buffers[n]) 
          if ret==False:
              break
          outs[n].write(buffers[n])
        frames_grabbed += 1

    # Release everything if job is finished

    for grabber in self.grabbers: grabber.quit()
    for out in outs: out.release()

def main():
  start = time.time()
  capture = Capture()
  capture.begin()
  while time.time() - start < 5.:
    time.sleep(0.1)
  capture.end()

if __name__ == "__main__":
  main()

