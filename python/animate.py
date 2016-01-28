#! /usr/bin/env python
import random
import gobject
import math
from math import *
from car import Dynamics, FakeCar
import filenames

import pygtk
pygtk.require('2.0')
import gtk, gobject, cairo

# Create a GTK+ widget on which we will draw using Cairo
class Screen(gtk.DrawingArea):

    # Draw in response to an expose-event
    __gsignals__ = { "expose-event": "override" }

    # Handle the expose-event by drawing
    def do_expose_event(self, event):

        # Create the cairo context
        cr = self.window.cairo_create()

        # Restrict Cairo to the exposed area; avoid extra work
        cr.rectangle(event.area.x, event.area.y,
                event.area.width, event.area.height)
        cr.clip()

        self.draw(cr, *self.window.get_size())

    def draw(self, cr, width, height):
        # Fill the background with gray
        cr.set_source_rgb(0.5, 0.5, 0.5)
        cr.rectangle(0, 0, width, height)
        cr.fill()

# GTK mumbo-jumbo to show the widget in a window and quit when it's closed
def run(Widget):
    window = gtk.Window()
    window.set_default_size(1000,500)
    window.connect("delete-event", gtk.main_quit)
    widget = Widget()
    widget.show()
    window.add(widget)
    window.present()
    gtk.main()

class World:
    def __init__(self):
        global args
        self.width = 20.
        self.height = 20.
        self.left = -self.width/2.
        self.right = self.width/2.
        self.top = -self.height/2.
        self.bottom = self.height/2.
        self.car = FakeCar(recording_file_path = args.infile)

    def move(self):
        if self.car.step() == False:
          self.car.reset()
    

class CarView:
  def draw(self, cr, car):
    oldmatrix = cr.get_matrix()
    cr.translate(car.ackerman.x,car.ackerman.y)
    cr.rotate(car.ackerman.heading)

    # car position x,y is at center of rear

    # car outline
    cr.set_line_width(0.01) # meters
    cr.set_source_rgb(0.5, 0.5, 0.5)
    cr.rectangle(0, car.width/-2.,  car.width, car.length)
    cr.fill()

    # car velocity arrow
    cr.set_source_rgb(0,1,0)
    cr.move_to(0,0)
    cr.set_line_width(0.1)
    cr.line_to(car.velocity, 0)
    cr.stroke()

    # car turn arrow
    s = cr.get_matrix()
    cr.set_source_rgb(0.5,0.5,1.)
    cr.set_line_width(0.1)
    cr.translate(car.length,0)
    cr.rotate(radians(car.wheels_angle()))
    cr.move_to(0,0)
    cr.line_to(car.length/2,0)
    cr.stroke()
    cr.set_matrix(s)

    # ping arrow
    cr.set_source_rgb(1.,.5,.5)
    cr.set_line_width(0.1)
    cr.translate(car.length,0)
    cr.move_to(0,0)
    cr.line_to(car.ping_distance(),0)
    cr.stroke()

    cr.set_matrix(oldmatrix)


class Transform(Screen):
    def __init__(self):
        self.world = World()
        super(Transform,self).__init__()
        self.draw_count = 0
        self.timer = gobject.timeout_add (10, self.idle_cb)


    def idle_cb(self):
        self.world.move()
        self.queue_draw()
        return True

    def draw(self, cr, width, height):
        self.draw_count += 1
        cr.set_source_rgb(0.5, 0.5, 0.5)
        cr.rectangle(0, 0, width, height)
        cr.fill()

        # draw a rectangle
        cr.set_source_rgb(1.0, 1.0, 1.0)
        cr.rectangle(15, 15, width - 30, height - 30)
        cr.fill()

        # set up a transform so that (0,0) to (world.width,world_height)
        # maps to middle of (20, 20) to (width - 40, height - 40)
        oldmatrix = cr.get_matrix()
        cr.translate(width/2, height/2)
        scale = min((width - 40) / self.world.width, (height - 40) / self.world.height)
        cr.scale(scale,scale)
        
        cr.move_to(0,0)
        cr.set_source_rgb(.5, .5, .5)
        
        # draw grid
        # get 1 pixel width
        w,_=cr.device_to_user_distance(0.5,0.5);
        cr.set_line_width(w) # meters
        
        y = self.world.top
        while y <= self.world.bottom:
          cr.move_to(y,self.world.left)
          cr.line_to(y, self.world.right)
          cr.stroke()
          y += 1
        x = self.world.left
        while x <= self.world.right:
          cr.move_to(self.world.top,x)
          cr.line_to(self.world.bottom,x)
          cr.stroke()
          x += 1
        # draw circle at origin
        cr.arc(0.,0.,.25,0.,2*math.pi);
        cr.stroke()

        carView = CarView()
        carView.draw(cr,self.world.car)
    
        cr.set_matrix(oldmatrix)
        cr.set_source_rgb(0,0,0)
        cr.set_font_size(11)
        cr.move_to(15,11)
        display_text ="{} frame: {} m/s: {:4.1f} esc: {:4} ax: {:4.1f}".format(
          args.infile,
          self.draw_count, 
          self.world.car.get_velocity_meters_per_second(),
          self.world.car.dynamics.esc,
          self.world.car.dynamics.ax
          )
          
        cr.show_text(display_text)


import argparse
parser = argparse.ArgumentParser(description = 'RC car control playback')
parser.add_argument(
  'infile',
  nargs='?',
#  type=argparse.FileType('r'),
  default= filenames.latest_filename('recordings','recording','csv'),
  help='csv file recorded with recorder')
args = parser.parse_args()

run(Transform)
