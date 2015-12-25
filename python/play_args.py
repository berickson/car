import argparse
parser = argparse.ArgumentParser(description = 'RC car control playback')
parser.add_argument(
  'infile',
  nargs='?',
#  type=argparse.FileType('r'),
  default='recording.csv',
  help='csv file recorded with recorder')
args = parser.parse_args()

print 'program logic starts here'
