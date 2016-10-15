#pragma once
/*
Usage:

Requires following in platformion.ini
lib_deps =
  I2Cdevlib-Core
  I2Cdevlib-MPU9150
*/

#include <stdint.h>
#include <math.h>
#include "helper_3dmath.h"
#define MPU9150_INCLUDE_DMP_MOTIONAPPS41
#include <MPU9150.h>
#include "Logger.h"

#define INTERRUPT_PIN 20
#define INTERRUPT_NUMBER 20

#include "Statistics.h"


class Mpu9150 {
public:
  MPU9150 mpu;

  void calibrate_as_horizontal();
  void calibrate_nose_up();
  void start_calibrate_at_rest(double pause_seconds, double test_seconds);

  void set_zero_orientation(Quaternion zeo);

  // MPU control/status vars
  bool dmpReady = false;  // set true if DMP init was successful
  bool initialReading = false; // set to true if we have initial reading
  uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
  uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
  uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
  uint16_t fifoCount;     // count of all bytes currently in FIFO
  uint8_t fifoBuffer[64]; // FIFO storage buffer
  unsigned long readingCount;

  // variables for at rest calibration
  bool at_rest_calibrating = false;
  uint32_t at_rest_calibration_start_millis = 0;
  uint32_t at_rest_calibration_end_millis = 0;
  double yaw_slope_rads_per_ms;
  double yaw_adjust_start_ms;



  // orientation/motion vars
  Quaternion q,qraw;           // [w, x, y, z]         quaternion container
  Quaternion down_adjust = Quaternion(1,0,0,0);
  Quaternion zero_adjust = Quaternion(1,0,0,0);
  VectorInt16 a, araw;
  VectorFloat gravity, graw;
  float ax,ay,az;
  float ax_bias,ay_bias,az_bias,rest_a_mag;

  VectorInt16 a0;
  VectorInt16 mag;
  float yaw_pitch_roll[3];
  float &yaw = yaw_pitch_roll[0];
  float &pitch = yaw_pitch_roll[1];
  float &roll = yaw_pitch_roll[2];

  Statistics ax_stats, ay_stats, az_stats,yaw_stats;
  void setup();
  float heading();
  void log_status();

  void execute();
  void zero();
};
