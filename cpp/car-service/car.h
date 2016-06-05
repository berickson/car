#ifndef CAR_H
#define CAR_H

#include <list>
#include <string>
#include "ackerman.h"
#include "dynamics.h"
#include "work_queue.h"

using namespace std;

class Car
{
public:
  Car(bool online = true);
  void reset_odometry();

  void add_listener(WorkQueue<Dynamics>*);
  void remove_listener(WorkQueue<Dynamics>*);

  string config_path = "/home/brian/car/python/car.ini";

  bool online = false;

  Ackerman ackerman;

  // state variables
  Dynamics dynamics;

  double velocity;
  double last_velocity;
  double heading_adjustment;
  int odometer_start;
  int odometer_front_left_start;
  int odometer_front_right_start;
  int odometer_back_left_start;
  int odometer_back_right_start;

  // calibrated measurements
  double meters_per_odometer_tick;
  double gyro_adjustment_factor;
  int center_steering_us;
  int min_forward_esc;
  int min_reverse_esc;
  int reverse_center_steering_us; // ?!
  double front_wheelbase_width_in_meters;
  double rear_wheelbase_width_in_meters;
  double wheelbase_length_in_meters;

  double length;
  double width;

private:
  void read_configuration(string path);
  list<WorkQueue<Dynamics>*> listeners;
};


void test_car();

#endif // CAR_H
