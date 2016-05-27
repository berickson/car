#!/usr/bin/env python2.7
# coding: utf-8

import os.path
import re
import string

import glob

def latest_filename(folder = '.', prefix = 'file', suffix = ''):
  filter = os.path.join(folder, prefix +"*" + suffix)
  matches = glob.glob(filter)
  matches.sort(reverse=True)
  return matches[0] if len(matches) > 0 else None

# returns a numbered, unique filename (with path) such as ./Recording_001.csv
# with a number one greater than any file in the given dir
def next_filename(folder = '.', prefix = 'file', suffix = ''):
  latest_number = 0
  last_file = latest_filename(folder, prefix, suffix)
  if last_file != None:
    m = re.match('.*'+re.escape(prefix+'_')+'(\d+)'+re.escape(suffix),last_file)
    latest_number = int(m.group(1)) if m != None else 0
  return os.path.join(folder, prefix +"_{:03}".format(latest_number + 1) + suffix)


class FileNames:
  def __init__(self, tracks_folder = '../tracks'):
    self.tracks_folder = os.path.abspath(tracks_folder)
  
  def get_track_folder(self,track_name):
    return os.path.join(self.tracks_folder, track_name)
    
  def get_track_names(self):
    track_names =  [p for p in os.listdir(self.tracks_folder) if os.path.isdir(self.get_track_folder(p))]
    track_names.sort()
    return track_names
    
  def get_routes_folder(self,track_name):
    return os.path.join(self.get_track_folder(track_name),'routes')

  def get_route_folder(self,track_name, route_name):
    return os.path.join(self.get_routes_folder(track_name),route_name)
    
  def get_route_names(self,track_name):
    if not os.path.exists(self.get_routes_folder(track_name)):
      return []
    route_names = [p for p in os.listdir(self.get_routes_folder(track_name)) if os.path.isdir(self.get_route_folder(track_name,p))]
    route_names.sort()
    return route_names
  
  def get_runs_folder(self, track_name, route_name):
    return os.path.join(self.get_route_folder(track_name,route_name),'runs')
  
  def get_run_folder(self, track_name, route_name, run_name):
    return os.path.join(self.get_runs_folder(track_name,route_name),run_name)

  def next_route_name(self,track_name):
    for route_name in list(string.ascii_uppercase):
      if not os.path.exists(self.get_route_folder(track_name, route_name)):
        return route_name
    raise 'could not find empty route'
  
  def recording_file_path(self,track_name, route_name, run_name = None):
    if run_name is None:
      folder = self.get_route_folder(track_name,route_name)
    else:
      folder = self.get_run_folder(track_name,route_name,run_name)
    return os.path.join(folder,'recording.csv')
    
  def stereo_video_file_paths(self,track_name, route_name, run_name):
    folder = self.get_run_folder(track_name, route_name, run_name)
    return [os.path.join(folder,'video_left.avi'),os.path.join(folder,'video_right.avi')]
  

    return os.path.join(self.get_route_folder(track_name,route_name),'recording.csv')
    
  def path_file_path(self,track_name, route_name, run_name = None):
    if run_name is None:
      folder = self.get_route_folder(track_name,route_name)
    else:
      folder = self.get_run_folder(track_name,route_name,run_name)
    return os.path.join(folder, 'path.csv')

  def next_run_name(self,track_name,route_name):
    for i in range(99):
      run_name = str(i+1)
      if not os.path.exists(self.get_run_folder(track_name, route_name, run_name)):
        return run_name
      i += 1
    raise 'could not find empty route'
        

if __name__ == '__main__':
  f = FileNames()
  print 'track names',f.get_track_names()
  tracks = f.get_track_names()
  track_name = f.get_track_names()[0]
  print 'routes folder for {0}'.format(track_name),f.get_routes_folder(track_name)
  print 'routes for {0}'.format(track_name),f.get_route_names(track_name)
  print 'next route name for {0}'.format(track_name),f.next_route_name(track_name)
  route_name = f.get_route_names(track_name)[0]
  print 'runs folder for {0}, {1}'.format(track_name,route_name), f.get_runs_folder(track_name, route_name)
  print 'next run name for {0}, {1}'.format(track_name,route_name), f.next_run_name(track_name, route_name)
  exit()
  print latest_filename('.','recording','.csv')
  print next_filename('.','recording','.csv')
  print latest_filename('.','other','.csv')
  print next_filename('.','other','.csv')
  print next_filename()
  print next_filename(folder='recordings',prefix='recording',suffix='.csv')

