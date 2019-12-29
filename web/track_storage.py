'''
access to track data
'''
import os
import os.path

class FolderBasedItem:
    ''' folder for a track '''
    def __init__(self, folder):
        self.folder = folder

    def get_name(self):
        ''' track name '''
        return os.path.split(self.folder)[1]

    def __str__(self):
        return self.get_name()

class Route(FolderBasedItem):
    '''route'''
    pass

class Track(FolderBasedItem):
    '''track'''
    def get_routes(self):
        ''' returns routes for this track '''
        routes_folder = os.path.join(self.folder, 'routes')
        return [self.get_route(name) for name in os.listdir(routes_folder)]

    def get_route(self, name):
        ''' returns route with given name '''
        return Route(os.path.join(self.folder, 'routes', name))


class TrackStorage:
    '''
    storage for all Tracks
    '''
    def __init__(self, folder='../tracks'):
        self.folder = os.path.abspath(folder)

    def get_track(self, name):
        ''' returns a single track '''
        return Track(os.path.join(self.folder, name))

    def get_tracks(self):
        ''' returns list of tracks '''
        return [self.get_track(name) for name in os.listdir(self.folder)]
