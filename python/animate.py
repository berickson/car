#! /usr/bin/env python
import random
import gobject
import math
from math import *
from Car import Dynamics

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


class robot:

    # --------

    # init: 
    #	creates robot and initializes location/orientation 
    #

    def __init__(self, length = 10.0):
        world_size = 100
        self.x = random.random() * world_size # initial x position
        self.y = random.random() * world_size # initial y position
        self.orientation = random.random() * 2.0 * pi # initial orientation
        self.length = length # length of robot
        self.bearing_noise  = 0.0 # initialize bearing noise to zero
        self.steering_noise = 0.0 # initialize steering noise to zero
        self.distance_noise = 0.0 # initialize distance noise to zero
    
    def __repr__(self):
        return '[x=%.6s y=%.6s orient=%.6s]' % (str(self.x), str(self.y), str(self.orientation))
    # --------
    # set: 
    #	sets a robot coordinate
    #

    def set(self, new_x, new_y, new_orientation):

        #if new_orientation < 0 or new_orientation >= 2 * pi:
        #    raise ValueError, 'Orientation must be in [0..2pi]'
        self.x = float(new_x)
        self.y = float(new_y)
        self.orientation = float(new_orientation)


    # --------
    # set_noise: 
    #	sets the noise parameters
    #

    def set_noise(self, new_b_noise, new_s_noise, new_d_noise):
        # makes it possible to change the noise parameters
        # this is often useful in particle filters
        self.bearing_noise  = float(new_b_noise)
        self.steering_noise = float(new_s_noise)
        self.distance_noise = float(new_d_noise)
    
    ############# ONLY ADD/MODIFY CODE BELOW HERE ###################

    # --------
    # move:
    #   move along a section of a circular path according to motion
    #
    
    def move(self, motion): # Do not change the name of this function
        result = robot()
        [alpha, d] = motion  # alpha is the steering angle, d is distance travelled
        d=float(d)
        alpha=float(alpha)
        #print 'd: '  + str(d)
        #print 'self.length: ' + str(self.length)
        #print 'alpha: ' + str(alpha)
        beta = d / self.length * tan(alpha)
        #print 'beta: ' + str(beta)
        if(abs(beta) < 0.001):
            result.x = self.x + d * cos(self.orientation)
            result.y = self.y + d * sin(self.orientation)
        else:
            R = d / beta
            cx = self.x - sin(self.orientation )*R # center of turn
            cy = self.y + cos(self.orientation)*R #
            result.x = cx + sin(self.orientation + beta) * R
            result.y = cy - cos(self.orientation + beta) * R
            
        result.orientation = (self.orientation+beta) % (2. * pi)
        result.length = self.length
        # ADD CODE HERE
        
        return result # make sure your move function returns an instance
                      # of the robot class with the correct coordinates.


class World:
    def __init__(self):
        self.width = 1800.
        self.height = 800.
        self.car = FakeCar(recording_file_path = 'data/recording_hall_and_back.csv')

    def move(self):
        self.car.move()
    
class FakeCar:
    def __init__(self, recording_file_path):
        self.recording_file_path = recording_file_path;
        self.reset()
        
    def reset(self):
        self.x = 250
        self.y = 250
        self.length = 60 # todo: use real car length in meters
        self.width = 30 # todo: use real car width in meters
        self.original_heading = 0
        self.heading = 0
        self.wheels_angle = 0
        self.velocity = 0
        self.dynamics_file = open(self.recording_file_path)
        self.dynamics = Dynamics()
        self.read_dynamics()
        self.original_heading = self.dynamics.heading * pi/180. 

    
    def read_dynamics(self):
        s = self.dynamics_file.readline()
        if not s:
          raise EOFError
        fields = s.split(',')
        self.dynamics.set_from_log(fields)

    def move(self):
        try:
          last_odometer = self.dynamics.odometer
          self.read_dynamics();
          self.velocity = self.dynamics.odometer - last_odometer
          self.heading = self.dynamics.heading * pi/180. - self.original_heading
    
          r = robot(self.length)
          r.set(self.x, self.y, self.heading)
          wheels_angle = 0 # todo: compute from dynamics, from structure in car
          r = r.move([0 , self.velocity])
          self.x = r.x
          self.y = r.y
          self.heading = r.orientation
        except:
          self.reset()


class CarView:
    def draw(self, cr, car):
          oldmatrix = cr.get_matrix()
          cr.translate(car.x,car.y)
          cr.rotate(car.heading)

          # car position x,y is at center of rear

          # car outline
          cr.set_source_rgb(0.5, 0.5, 0.5)
          cr.rectangle(0, car.width/-2.,  car.length, car.width)
          cr.fill()

          # car velocity arrow
          cr.set_source_rgb(0,1,0)
          cr.move_to(0,0)
          cr.line_to(car.velocity, 0)
          cr.stroke()

          # car turn arrow
          cr.set_source_rgb(0.5,0.5,1.)
          cr.translate(car.length,0)
          cr.rotate(car.wheels_angle)
          cr.move_to(0,0)
          cr.line_to(10,0)
          cr.stroke()

          cr.set_matrix(oldmatrix)


class Transform(Screen):
    def __init__(self):
        self.world = World()
        super(Transform,self).__init__()
        self.draw_count = 0
        self.timer = gobject.timeout_add (30, self.idle_cb)


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

        # set up a transform so that (0,0) to (world.width,world_heigth)
        # maps to (20, 20) to (width - 40, height - 40)
        oldmatrix = cr.get_matrix()
        cr.translate(20, 20)
        cr.scale((width - 40) / self.world.width, (height - 40) / self.world.height)


        carView = CarView()
        carView.draw(cr,self.world.car)
    
        cr.set_matrix(oldmatrix)
        cr.set_source_rgb(0,0,0)
        cr.set_font_size(11)
        cr.move_to(15,11)
        cr.show_text(str(self.draw_count))

run(Transform)
