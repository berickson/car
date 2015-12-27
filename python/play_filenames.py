import os.path
import re

import glob

def latest_filename(dir,prefix,suffix):
  filter = os.path.join(dir, prefix +"*" + "." + suffix)
  matches = glob.glob(filter)
  if matches == None:
    return None
  matches.sort(reverse=True)
  return matches[0]

# returns a numbered, unique filename (with path) such as ./Recording_001.csv
def unique_filename(dir,prefix,suffix):
  last_file = latest_filename(dir,prefix,suffix)
  m = re.match('.*'+prefix+'\_(\d+)\.'+suffix,last_file)
  if m == None:
    latest_number = 0
  else:
    latest_number = int(m.group(1))

  return os.path.join(dir, prefix +"_{:03}".format(latest_number + 1) + "." + suffix)

print latest_filename('.','recording','csv')
print unique_filename('.','recording','csv')


