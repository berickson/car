'''
tests for Tracks
'''
import unittest


from tracks import TrackStorage


class TracksTests(unittest.TestCase):
    ''' tests for tracks '''
    def test_get_tracks(self):
        ''' list of tracks '''
        print("Hello, tests")
        storage = TrackStorage()
        tracks = storage.get_tracks()
        print(",".join([t.get_name() for t in tracks]))
        self.assertTrue(len(tracks) > 0)

    def test_get_track(self):
        ''' tests get_track '''
        track = TrackStorage().get_track('desk')
        self.assertIsNotNone(track)

if __name__ == '__main__':
    unittest.main()
