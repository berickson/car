import time
import threading

class Interval:
  def __init__(self, interval=1.0):
    self.last_time = time.time()
    self.interval = interval

  def occurred(self):
     new_time = time.time()
     occurred =  ((self.last_time % self.interval) + (new_time - self.last_time)) >= self.interval
     self.last_time = new_time
     return occurred
    
  

class RuntimeStats:
  def __init__(self,stat_name=""):
    self.stat_name = stat_name
    self.count = 0
    t = time.time()
    self.start_time = t
    self.latest_time = t
    self.last_interval_start= t
    self.interval_count = 0
    self.interval = Interval(1.0)
    self.rate = float('NaN')
  
  def _on_interval(self):
    t = time.time()
    count = self.count
    
    elapsed = t - self.last_interval_start
    self.rate = (count - self.interval_count) / elapsed
    self.last_interval_start = t
    self.interval_count = self.count
  
  def increment(self):
    self.count = self.count + 1
    self.latest_time = time.time()
    if self.interval.occurred():
      self._on_interval()

  def __repr__(self):
    elapsed = self.latest_time-self.start_time
    if elapsed == 0.:
      return "not time has elapsed"
    s = ("stat_name: {}\n"
         "count: {}\n"
         "avg per second: {}\n").format(
           self.stat_name,
           self.count,
           self.rate)
    return s

class FrameGrabber:
  lock = threading.Lock()
  def __init__(self,camera):
    self._quit = False
    self.camera = camera
    self.img = None
    self.grab_stats = RuntimeStats('grabs')
    self.read_stats = RuntimeStats('reads')
    
  def stats(self):
    return str(self.grab_stats) + str(self.read_stats)
    
  def _grab_thread(self):
    while self._quit == False:
      FrameGrabber.lock.acquire()
      try:
        self.camera.grab()
        self.grab_stats.increment()
      finally:
        FrameGrabber.lock.release()
      time.sleep(0.001)
    
  def go(self):
    self._thread = threading.Thread(target = self._grab_thread)
    self._thread.start()
  
  def quit(self):
    self._quit = True
    self._thread.join()
    
  def read(self, buffer = None):
    FrameGrabber.lock.acquire()
    try:
      if buffer == None:
        retval,img = self.camera.retrieve(self.img)
      else:
        retval,img = self.camera.retrieve(buffer)
      if retval: 
        self.img = img
        self.read_stats.increment()
    finally:
      FrameGrabber.lock.release()
    return (retval,img)

