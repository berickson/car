#pragma once

//#include "i2c_t3.h"
#include "I2Cdev.h"
#include "helper_3dmath.h"
#include "MPU6050.h"
#include "vector_math.h"
#include "Logger.h"

#define INTERRUPT_PIN 12
#define INTERRUPT_NUMBER 12


class Mpu9150 {
public:
  MPU6050 mpu;

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
  Quaternion q;           // [w, x, y, z]         quaternion container
  Quaternion q0;          // initial quaternion
  VectorInt16 aa;         // [x, y, z]            accel sensor measurements
  VectorInt16 a0;
  VectorInt16 mag;
  VectorFloat gravity;    // [x, y, z]            gravity vector

  void setup();
  double ground_angle();
  void log_status();

  void execute();
  void zero();
};
