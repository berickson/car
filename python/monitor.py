#!/usr/bin/env python2.7

import serial
import sys

o = sys.stdout
s = serial.Serial("/dev/ttyACM0")
while True:
    if(s.inWaiting() > 0):
        while(s.inWaiting() > 0):
            o.write(s.readline())
        o.flush()
