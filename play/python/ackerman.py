# coding=utf-8
'''
Ackerman steering

https://pdfs.semanticscholar.org/5849/770f946e7880000056b5a378d2b7ac89124d.pdf
'''

import math

class Pose2d:
    ''' pose of an object in 2d '''
    def __init__(self, x=0, y=0, theta=0):
        self.x = float(x)
        self.y = float(y)
        self.theta = float(theta)

    def __repr__(self):
        return '({}, {}, {}°)'.format(self.x, self.y, self.theta * 180/math.pi)

    def rotate_around_point(self, x, y, theta):
        ''' rotates self around point x,y at angle theta '''
        dx = self.x - x
        dy = self.y - y
        rx = dx * math.cos(theta) - dy * math.sin(theta)
        ry = dy * math.cos(theta) + dx * math.sin(theta)
        self.x = rx + x
        self.y = ry + y
        self.theta += theta

    def relative_point(self, rx, ry):
        x = self.x + rx * math.cos(self.theta) - ry * math.sin(self.theta)
        y = self.y + ry * math.cos(self.theta) + rx * math.sin(self.theta)
        return (x, y)

    def rotate_around_relative_point(self, rx, ry, theta):
        x, y = self.relative_point(rx, ry)
        self.rotate_around_point(x, y, theta)

    def move(self, dx, dy):
        ''' moves self by offset '''
        self.x += dx
        self.y += dy

    def move_relative(self, dx, dy):
        ''' moves distance relative to pose '''
        p = Pose2d(dx, dy, 0)
        p.rotate_around_point(0, 0, self.theta)
        self.x += p.x
        self.y += p.y


class Ackerman:
    ''' Ackerman Steering'''
    def __init__(self, x=0, y=0, theta=0, wheelbase_length=1):
        self.pose = Pose2d(x, y, theta)
        self.wheelbase_length = wheelbase_length
        self.steering_angle = 0

    def move_front_wheel(self, wheel_offset_left, distance, d_theta):
        '''move wheel at wheel_x, wheel_y distance ds while car rotates d_theta'''

        if d_theta != 0.:
            # First, calculate radius of rotation for the circle that the wheel is on
            r_front = distance / d_theta

            # it's impossible to have a turn radius less than the wheelbase length
            if math.fabs(r_front) < self.wheelbase_length:
                rho = math.pi/2
            else:
                # Now find the turn radius of the associated rear wheel
                r_rear = math.sqrt(r_front**2 - self.wheelbase_length**2)

                # Now we can find the steering angle rho
                rho = math.asin(self.wheelbase_length/r_rear)

            if (d_theta < 0) != (distance < 0):
                rho = -rho
                r_rear = - r_rear

            self.steering_angle = rho

            self.pose.rotate_around_relative_point(0, wheel_offset_left+r_rear, d_theta)

        else:
            rho = 0.
            self.pose.move_relative(distance, 0)

        return rho
