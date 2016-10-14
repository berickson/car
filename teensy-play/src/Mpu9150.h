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


class Mpu9150 {
public:
  MPU9150 mpu;
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

  // orientation/motion vars
  Quaternion q,qraw;           // [w, x, y, z]         quaternion container
  Quaternion zero_adjust = Quaternion(1,0,0,0);
  VectorInt16 a, araw;
  VectorFloat gravity, graw;
  float ax,ay,az;
  VectorInt16 a0;
  VectorInt16 mag;
  float yaw_pitch_roll[3];
  float &yaw = yaw_pitch_roll[0];
  float &pitch = yaw_pitch_roll[1];
  float &roll = yaw_pitch_roll[2];

  void setup();
  float heading();
  void log_status();

  void execute();
  void zero();
};
