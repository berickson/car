import os.path
import re

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

if __name__ == '__main__':
  print latest_filename('.','recording','.csv')
  print next_filename('.','recording','.csv')
  print latest_filename('.','other','.csv')
  print next_filename('.','other','.csv')
  print next_filename()
  print next_filename(folder='recordings',prefix='recording',suffix='.csv')



