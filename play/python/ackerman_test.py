''' unit tests for ackerman module'''

import math
import unittest
import ackerman

class PoseTests(unittest.TestCase):
    ''' unit tests for Pose2d class '''
    def test_rotate_180(self):
        '''rotate 180 degrees'''
        pose = ackerman.Pose2d(5, 7, 3)
        pose.rotate_around_point(0, 0, math.pi)
        self.assertAlmostEqual(pose.x, -5)
        self.assertAlmostEqual(pose.y, -7)
        self.assertAlmostEqual(pose.theta, 3 + math.pi)

    def test_rotate_90(self):
        '''rotate 180 degrees'''
        pose = ackerman.Pose2d(5, 7, 3)
        pose.rotate_around_point(0, 0, math.pi/2.)
        self.assertAlmostEqual(pose.x, -7)
        self.assertAlmostEqual(pose.y, 5)
        self.assertAlmostEqual(pose.theta, 3 + math.pi/2)

    def test_around_x(self):
        ''' rotate origin around point '''
        pose = ackerman.Pose2d(0, 0, 0)
        pose.rotate_around_point(1, 0, math.pi/2)
        self.assertAlmostEqual(pose.x, 1)
        self.assertAlmostEqual(pose.y, -1)
        self.assertAlmostEqual(pose.theta, math.pi/2)
        pose.rotate_around_point(1, 0, -math.pi/2)
        self.assertAlmostEqual(pose.x, 0)
        self.assertAlmostEqual(pose.y, 0)
        self.assertAlmostEqual(pose.theta, 0)


    def test_move(self):
        ''' simple movement '''
        pose = ackerman.Pose2d(1, 1, 0)
        pose.move(-.5, .25)
        self.assertAlmostEqual(pose.x, 0.5)

    def test_relative_point(self):
        pose = ackerman.Pose2d(2, 1, math.pi/2)
        x, y = pose.relative_point(.25, .5)
        self.assertAlmostEqual(y, 1.25)
        self.assertAlmostEqual(x, 1.5)


    def test_move_relative(self):
        pose = ackerman.Pose2d(0, 0, 0)
        pose.move_relative(.25, -.15)
        self.assertAlmostEqual(pose.x, 0.25)
        self.assertAlmostEqual(pose.y, -0.15)

        pose = ackerman.Pose2d(2, 1, math.pi/2)
        pose.move_relative(.5, .25)
        self.assertAlmostEqual(pose.x, 1.75)
        self.assertAlmostEqual(pose.y, 1.5)




class AckermanTests(unittest.TestCase):
    def test_move_front_wheel(self):
        car = ackerman.Ackerman(wheelbase_length=1)
        print(car.pose)
        car.move_front_wheel(wheel_offset_left=0, distance=0.25, d_theta=2.*math.pi/180)
        car.move_front_wheel(0, -0.25, -2.*math.pi/180)
        print(car.pose)
        self.assertAlmostEqual(car.pose.x, 0)
        self.assertAlmostEqual(car.pose.y, 0)
        self.assertAlmostEqual(car.pose.theta, 0)

    def test_move_offset(self):
        car1 = ackerman.Ackerman(wheelbase_length=0)
        car1.move_front_wheel(0.5, .5, 10 * math.pi / 180)
        print('left wheel moved', car1.pose)
        car2 = ackerman.Ackerman(wheelbase_length=0)
        car2.move_front_wheel(-0.5, .5, 10 * math.pi / 180)
        self.assertGreater(car1.pose.x, car2.pose.x)
        print('right wheel moved', car2.pose)


if __name__ == '__main__':
    unittest.main()
