#include <Servo.h>

struct PwmInput {
  unsigned long last_trigger_us = 0;
  unsigned long pulse_width_us = 0;
  
  unsigned long max_pulse_us = 3000;
  unsigned long min_pulse_us = 500;
  unsigned long last_pulse_ms = 0;

  
  // milliseconds without a pulse to consider a timeout
  int timeout_ms = 100;
  int pin;
  
  void attach(int pin) {
    this->pin = pin;
    last_trigger_us = 0;
    pulse_width_us = 0;
    pinMode(pin, INPUT);
  }
  
  void handle_change() {
    unsigned long us = micros();
    if(digitalRead(pin)) {
      last_trigger_us = us;
    }
    else {
      unsigned long width = us - last_trigger_us;
      // only accept pulses in acceptable range
      if(width >= min_pulse_us && width <= max_pulse_us) {
        pulse_width_us = width;
        last_pulse_ms = millis();
      }
    }
  }
  
  bool is_valid() {
    return millis() - last_pulse_ms < timeout_ms;
  }
  
  void trace() {
    Serial.print("PWM pin ");
    Serial.print(pin);
    Serial.print(" ");
    Serial.print(pulse_width_us);
    if(is_valid())
      Serial.print(" valid");
    else
      Serial.print(" invalid");
  }
};


struct TriangleWave {
  int period_ms = 1000;
  int max_value = 1000;
  int min_value = 2000;
  int origin_ms;
  
  void init() {
    origin_ms = millis();
  }
  
  int value() {
    unsigned long now_ms = millis();
    int range = max_value - min_value;

    double cycle_portion = (( now_ms - origin_ms ) % period_ms) / (double) period_ms;
    if(cycle_portion > 0.5)
        cycle_portion = 1-cycle_portion;
    return (2 * cycle_portion * range + min_value);
    
  }
};


// on the micro, interrupts must go on pins 2 and 3
// interrupt 0 = pin 2
// interrupt 1 = pin 3
#define RX_STR 2
#define RX_SPD 3


// computer steering
#define U_STR 8


Servo steering;
PwmInput rx_str;
PwmInput rx_spd;

volatile unsigned int rx_str_trig = 0;
volatile unsigned int rx_str_width = 0;

const int leftmost = 1000;
const int rightmost = 2000;
int increment = 1;
int last_steering_update_millis = 0;
int steering_update_rate_millis = 20;
int steer = leftmost;

void rx_str_handler() {
  rx_str.handle_change();
}

void rx_spd_handler() {
  rx_spd.handle_change();
}


TriangleWave triangle_wave;

void setup() {
  steering.attach(U_STR);
  rx_str.attach(RX_STR);
  rx_spd.attach(RX_SPD);
  
  attachInterrupt(0, rx_str_handler, CHANGE);
  attachInterrupt(1, rx_spd_handler, CHANGE);
  
  triangle_wave.min_value = 1200;
  triangle_wave.max_value = 1800;
  triangle_wave.init();

  Serial.begin(9600);
  Serial.println("hello, steering!");
}

void loop() {
  /*
  rx_str.trace();
  Serial.print("    ");
  rx_spd.trace();
  Serial.println();
  */
 
  int loop_time_millis = millis();

/*
  Serial.print("Triangle: ");
  Serial.println(triangle_wave.value());
*/  
  steering.writeMicroseconds(triangle_wave.value());
 
}
