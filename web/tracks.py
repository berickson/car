'''
access to track data
'''
import os
import os.path
import time
from operator import methodcaller

class FolderBasedItem:
    ''' folder for a track '''
    def __init__(self, folder):
        self.folder = folder

    def get_name(self):
        ''' track name '''
        return os.path.split(self.folder)[1]

    def get_time(self):
        ''' track modified time '''
        return time.gmtime(os.path.getmtime(self.folder))

    def __str__(self):
        return self.get_name()

class Run(FolderBasedItem):
    pass

class Route(FolderBasedItem):
    '''route'''
    def get_run(self, name):
        return Run(os.path.join(self.folder, 'runs', name))

    def get_runs(self):
        ''' returns routes for this track '''
        runs_folder = os.path.join(self.folder, 'runs')
        try:
            runs = [self.get_run(name) for name in os.listdir(runs_folder)]
        except:
            runs = []
        runs = sorted(runs, key=methodcaller('get_name'))
        return runs

class Track(FolderBasedItem):
    '''track'''
    def get_routes(self):
        ''' returns routes for this track '''
        routes_folder = os.path.join(self.folder, 'routes')
        try:
            routes = [self.get_route(name) for name in os.listdir(routes_folder)]
        except:
            routes = []
        routes = sorted(routes, key=methodcaller('get_name'))
        return routes

    def get_route(self, name):
        ''' returns route with given name '''
        return Route(os.path.join(self.folder, 'routes', name))


class TrackStorage:
    '''
    storage for all Tracks
    '''
    def __init__(self, folder='/home/pi/car/tracks'):
        self.folder = folder #os.path.abspath(folder)

    def get_track(self, name):
        ''' returns a single track '''
        return Track(os.path.join(self.folder, name))

    def add_track(self, name):
        ''' adds a single track '''
        os.mkdir(os.path.join(self.folder, name))

    def get_tracks(self):
        ''' returns list of tracks '''
        tracks = [self.get_track(name) for name in os.listdir(self.folder)]
        tracks = sorted(tracks, key=methodcaller('get_name'))
        return tracks

def get_run_settings_path():
    return "/home/pi/car/bin/run_settings.json"
