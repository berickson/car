import sys,tty,termios
import datetime
import time
import dateutil.parser
from select import select
from Car import Car, Dynamics, degrees_diff
import filenames
from ackerman import Ackerman
from math import *
import copy

class Recording:
  def __init__(self, recording_file_path = None):
    if recording_file_path == None:
      folder = 'recordings'
      prefix = 'recording'
      suffix = '.csv'
      recording_file_path = filenames.latest_filename(folder=folder,prefix=prefix,suffix=suffix)
    self.recording = open(recording_file_path,'r');
    
  # reads lines from file, returns False if no more lines
  def read(self):
    self.dynamics = Dynamics()
    s = self.recording.readline()
    if not s:
      return False
    fields = s.split(',')
    self.dynamics.set_from_log(fields)
    return True
  
  def current(self):
    return self.dynamics

