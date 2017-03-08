'''
Ackerman steering

https://pdfs.semanticscholar.org/5849/770f946e7880000056b5a378d2b7ac89124d.pdf

delta: angle wheel is being steered
v : velocity of center of rear wheels
theta : angle car is making with the x axis

x_dot = v * cos(theta)
y_dot = v * sin(theta)
theta_dot = (v / L) * tan(delta)


'''
import math

class Ackerman:
    ''' Ackerman Steering'''
    def __init__(self, x_position=0, y_position=0, theta=0):
        self.x_position = x_position
        self.y_position = y_position
        self.theta = theta

    def wheel_steering_angle(self, wheelbase_length, distance, d_theta):
        '''move wheel at wheel_x, wheel_y distance ds while car rotates d_theta'''

        if d_theta != 0.:
            # First, calculate radius of rotation for the circle that the wheel is on
            r_front = distance / d_theta

            # it's impossilbe to have a turn radius less than the wheelbase length
            if math.fabs(r_front) < wheelbase_length:
                rho = math.pi/2
            else:
                # Now find the turn radius of the associated rear wheel
                r_rear = math.sqrt(r_front**2 - wheelbase_length**2)

                # Now we can find the steering angle rho
                rho = math.asin(wheelbase_length/r_rear)
            if d_theta < 0:
                rho = -rho
        else:
            rho = 0.
        return rho

def test():
    ''' main test '''
    ackerman = Ackerman()
    for wheelbase_length in [1., 2., 10.]:
        for distance in [5.,]:
            for d_theta_degrees in [-10.]:
                steering_angle = ackerman.wheel_steering_angle(wheelbase_length, distance, d_theta_degrees * math.pi/180)
                print('L: {:.2f} distance:{:.2f} d_theta:{:.2f} steering_angle:{:.2f}'.format(
                    wheelbase_length,
                    distance,
                    d_theta_degrees,
                    steering_angle * 180./math.pi))

if __name__ == '__main__':
    test()
