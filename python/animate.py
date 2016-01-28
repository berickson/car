#! /usr/bin/env python
import random
# http://cairographics.org/documentation/pycairo/3/reference/index.html
import gobject
import math
from math import *
from car import Dynamics, FakeCar
import filenames
import geometry

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
        self.left = -2.
        self.right = 2.
        self.top = -2.
        self.bottom = 2.
        self.car = FakeCar(recording_file_path = args.infile)
        
    def width(self):
        return abs(self.right-self.left)
        
    def height(self):
        return abs(self.top-self.bottom)
    
    def inflate_to_include(self,x,y):
      self.left = min(self.left, x)
      self.right = max(self.right, x)
      self.top = min(self.top,y)
      self.bottom = max(self.bottom,y)

    def move(self):
        if self.car.step() == False:
          self.car.reset()
        self.inflate_to_include(self.car.ackerman.x+2,self.car.ackerman.y+2)
        self.inflate_to_include(self.car.ackerman.x-2,self.car.ackerman.y-2)
    

class CarView:
  def draw(self, cr, car):
    oldmatrix = cr.get_matrix()
    cr.translate(car.ackerman.x,car.ackerman.y)
    cr.rotate(car.ackerman.heading)

    # car position x,y is at center of rear

    # car outline
    cr.set_line_width(device_pixels(cr,1)) 
    cr.set_source_rgb(0.5, 0.5, 0.5)
    cr.rectangle(0, car.width/-2.,  car.length, car.width, )
    cr.fill()

    # car velocity arrows
    m2 = cr.get_matrix()
    cr.translate(car.length/2,0)
    cr.set_source_rgb(0,1,0)
    cr.move_to(0,0)
    cr.set_line_width(device_pixels(cr,2))
    cr.line_to(car.dynamics.ax, 0)
    cr.stroke()
    cr.move_to(0,0)
    cr.line_to(0, car.dynamics.ay)
    cr.stroke()
    cr.set_matrix(m2)

    # car turn arrow
    s = cr.get_matrix()
    cr.set_source_rgb(0.5,0.5,1.)
    cr.set_line_width(device_pixels(cr,1))
    cr.translate(car.length,0)
    cr.rotate(radians(car.wheels_angle()))
    cr.move_to(0,0)
    cr.line_to(car.length/2,0)
    cr.stroke()
    cr.set_matrix(s)

    # ping arc
    cr.set_source_rgb(.5,.5,.5)
    cr.set_line_width(device_pixels(cr,1))
    cr.translate(car.length,0)
#    cr.line_to(car.ping_distance(),0)
    cr.new_sub_path()
    cr.arc(car.length,0,car.ping_distance(),-0.261799, 0.261799)
    cr.new_sub_path()
    cr.arc(car.length,0,car.ping_distance()*0.9,-0.261799*.8, 0.261799*.8)
    cr.stroke()

    cr.set_matrix(oldmatrix)

def device_pixels(cr, x):
  x = float(x)
  px,py = cr.device_to_user_distance(x,0.);
  return geometry.length(px,py)

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
        cr.set_source_rgb(1,1,1)
        cr.set_font_size(11)
        cr.move_to(15,11)
        display_text ="{} frame: {:4} x: {:5.2f} y: {:5.2f} m/s: {:4.1f} esc: {:4} ax: {:4.1f} ay: {:4.1f}".format(
          args.infile,
          self.draw_count, 
          self.world.car.ackerman.x,
          self.world.car.ackerman.y,
          self.world.car.get_velocity_meters_per_second(),
          self.world.car.dynamics.esc,
          self.world.car.dynamics.ax,
          self.world.car.dynamics.ay,
          )
          
        cr.show_text(display_text)
        #set clip rectangle for main display_text
        cr.set_source_rgb(1.0, 1.0, 1.0)
        cr.rectangle(15, 15, width - 30, height - 30)
        cr.fill()
        cr.rectangle(15, 15, width - 30, height - 30)
        cr.clip()


        # set up a transform so that (0,0) to (world.width,world_height)
        # maps to middle of (20, 20) to (width - 40, height - 40)
        oldmatrix = cr.get_matrix()
        cr.translate(width*(-self.world.left / self.world.width()), 
        -height*self.world.top / self.world.height())
        scale = max((width - 40) / self.world.width(), (height - 40) / self.world.height())
        cr.scale(scale,-scale)
        
#        cr.move_to(0,0)
        
        # draw grid
        cr.set_line_width(device_pixels(cr,0.5)) 
        cr.set_source_rgb(.75, .75, .75)
        
        y = math.floor(self.world.top)
        while y <= math.ceil(self.world.bottom):
          cr.move_to(self.world.left, y)
          cr.line_to(self.world.right, y )
          cr.stroke()
          y += 1
        x = math.floor(self.world.left)
        while x <= math.ceil(self.world.right):
          cr.move_to(x,self.world.top)
          cr.line_to(x, self.world.bottom)
          cr.stroke()
          x += 1
        # draw circle at origin
        cr.arc(0.,0.,.25,0.,2*math.pi);
        cr.stroke()

        carView = CarView()
        carView.draw(cr,self.world.car)
    
        cr.set_matrix(oldmatrix)


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
