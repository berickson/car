#include <Servo.h>

#define STEERING_PIN 4

Servo steering;

void setup() {
  steering.attach(STEERING_PIN);
}

void loop() {
  int leftmost = 1000;
  int rightmost = 2000;
  
  int increment = 1;
  int delay_ms = 20; // match the duty cycle
  int steer = leftmost;
  
  for(;;) {
    for(steer = leftmost; steer < rightmost; steer += increment) {
      steering.writeMicroseconds(steer);
      delay(delay_ms);
    }
  
    for(steer = rightmost; steer > leftmost; steer -= increment) {
      steering.writeMicroseconds(steer);
      delay(delay_ms);
    }
  }
}
