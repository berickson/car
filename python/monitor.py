#!/usr/bin/env python2.7

import serial
import sys
import datetime
import time
import glob
from select import select

def kbhit():
  dr,dw,de = select([sys.stdin],[],[],0)
  return dr <> []

while True:
    for usb_path in glob.glob('/dev/ttyACM*'):
        try:
            o = sys.stdout
            s = serial.Serial(usb_path)
            while True:
                if(kbhit()):
                   c = raw_input('enter command:')
                   s.write('{0}\n'.format(c))
                   
                if(s.inWaiting() > 0):
                    while(s.inWaiting() > 0):
                        l = s.readline()
                        ts = datetime.datetime.now()
                        o.write("{0}\t{1}".format(ts,l))
                        o.flush()
        except IOError:
            time.sleep(1.0)

