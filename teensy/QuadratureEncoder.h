#pragma once

class QuadratureEncoder {
  public:
  const int pin_sensor_a; // sensor that triggers first for forward
  const int pin_sensor_b; 
  unsigned long last_odometer_change_us = 0;
  long odometer = 0;
  
  QuadratureEncoder(int _pin_sensor_a, int _pin_sensor_b):
    pin_sensor_a(_pin_sensor_a),
    pin_sensor_b(_pin_sensor_b)
  {
  }
  
  void sensor_a_changed() {
    last_odometer_change_us=micros();
    if(digitalRead(pin_sensor_a)==digitalRead(pin_sensor_b)){
      odometer-=2;
    } else {
      odometer+=2;
    }
  }
  
  void sensor_b_changed() {
      /*
    last_odometer_change_us=micros();
    if(digitalRead(pin_sensor_a)==digitalRead(pin_sensor_b)){
      ++odometer;
    } else {
      --odometer;
    }
    */
  }
};
