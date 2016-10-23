#include "Arduino.h"
#include "helper_3dmath.h"

#include <MPU9150_9Axis_MotionApps41.h>

#include "Mpu9150.h"


const uint8_t pin_led = 13;
Mpu9150 mpu9150;

void setup(){
  Serial.begin(250000);
  delay(1000);
  Wire.begin();
  TRACE_MPU = false;
  delay(10);
  pinMode(pin_led, OUTPUT);
  digitalWrite(pin_led,1);
  //delay(10);
  Serial.println("setting orientatino");
  //[0.70, -0.18, -0.67, -0.17]
  //Quaternion zero(0.674316,-0.0788574,-0.731384,0.0640259);
  Quaternion zero(0.72, -0.00, -0.70,  0.01);
  mpu9150.set_zero_orientation(zero);
  Serial.println("inside setup");
  mpu9150.setup();
  Serial.println("setup complete");
  mpu9150.enable_interrupts(33);

}

struct LoopTracker {
  unsigned long last_loop_ms = 0;
  unsigned long loop_ms = 0;
  unsigned long loop_count = 0;
  // todo: run execute once only at the top of each loop
  void execute() {
    ++loop_count;
    last_loop_ms = loop_ms;
    loop_ms = millis();
  }
  // returns true if loop time just passed through n ms boundary
  bool every_n_ms(unsigned long ms) {
    return (last_loop_ms % ms) + (loop_ms - last_loop_ms) >= ms;
  }
} loop_tracker;


int mpu_reporting_ms = 1000;
bool calibrating_circle = false;
double delta_yaw = 0.0;
double last_yaw = 0.0;
bool verbose_logging = false;

void print_mpu_status() {
  Serial.print((String) (millis()/1000) +  " ypr[" + mpu9150.heading() + "," + mpu9150.pitch * 180./PI + "," + mpu9150.roll * 180./PI + "]");
  Serial.print((String) " a[" + mpu9150.ax+ "," + mpu9150.ay+ ","+ mpu9150.az + "]");
  if( verbose_logging ) {
    Serial.print((String) " z[" + mpu9150.zero_adjust.w+ "," + mpu9150.zero_adjust.x+ ","+ mpu9150.zero_adjust.y+ ","+ mpu9150.zero_adjust.z + "]");
    Serial.print((String) " qr[" + mpu9150.qraw.w+ "," + mpu9150.qraw.x+ ","+ mpu9150.qraw.y+ ","+ mpu9150.qraw.z + "]");
    Serial.print((String) " ar[" + mpu9150.araw.x+ "," + mpu9150.araw.y+ ","+ mpu9150.araw.z + "]");
    Serial.print((String) " q[" + mpu9150.q.w+ "," + mpu9150.q.x+ ","+ mpu9150.q.y+ ","+ mpu9150.q.z + "]");
    Serial.print((String) " g[" + mpu9150.gravity.x+ "," + mpu9150.gravity.y+ ","+ mpu9150.gravity.z + "]");
  }
  Serial.println();
}

void print_help() {
  Serial.println("? - print help text");
  Serial.println("g - calibrate based on gravity while held level");
  Serial.println("r - calibrate drift at rest");
  Serial.println("t - calibrate front direction while tilting the front up");
  Serial.println("d - dump (print) calibration constants");
  Serial.println("m - log readings once a minute");
  Serial.println("s - log readings once a second");
  Serial.println("v - toggle verbose logging");
  Serial.println("l - manually log single reading");
  Serial.println("c - begin / end circle calibration mode");
}

void begin_circle_calibration() {
  calibrating_circle = true;
  last_yaw = mpu9150.yaw;
  delta_yaw = 0.0;
}

void end_circle_calibration() {
  calibrating_circle = false;
}

void toggle_verbose_logging() {
  verbose_logging = !verbose_logging;
  Serial.print("Verbose logging ");
  Serial.println( verbose_logging ? "ON" : "OFF");
}

void dump_calibration_constants() {

}



void loop() {
  int command = 0;
  while(Serial.available()) {
    command = Serial.read();
    if(command) {
      switch (command) {
      case '?':
        print_help();
        break;

      case 'd':
        dump_calibration_constants();
        break;

      case 'g':
        mpu9150.calibrate_as_horizontal();
        break;

      case 't':
        mpu9150.calibrate_nose_up();
        break;

      case 'r':
        mpu9150.start_calibrate_at_rest(1,60);
        break;

      case 'm':
        mpu_reporting_ms = 60*1000;
        Serial.println("set to print MPU status on every minute");
        break;

      case 's':
        mpu_reporting_ms = 1000;
        Serial.println("set to print MPU status on every second");
        break;

      case 'v':
        toggle_verbose_logging();
        break;

      case 'l':
        mpu_reporting_ms = 0;
        print_mpu_status();
        break;
      case 'c':
        if(calibrating_circle) {
          end_circle_calibration();
        } else {
          begin_circle_calibration();
        }
        break;

      default:
        break;
      }
    }
  }
  if(calibrating_circle) {
    double yaw = mpu9150.yaw;
    double d = yaw-last_yaw;
    last_yaw = yaw;
    if(d < -PI) {
      d += 2 * PI;
    } else if (d > PI) {
       d -= 2 * PI;
    }
    int estimated_circles = floor((fabs(delta_yaw) + PI)/(2*PI));
    if(delta_yaw < 0) {
      estimated_circles *= -1;
    }
    delta_yaw += d;
    double total_error = delta_yaw - 2 * PI * estimated_circles;
    double error_per_circle = estimated_circles ==0? 0 : total_error / estimated_circles;

    if( loop_tracker.every_n_ms(1000)) {
        Serial.println((String) "press 'c' to finalize circle calibration delta_yaw: " + delta_yaw * 180. / PI);
        Serial.println((String) "estimated circles: " + estimated_circles);
        Serial.println((String) "Error total: " + total_error * 180. / PI);
        Serial.println((String) "Error per circle: " + error_per_circle * 108. / PI);
    }
  }

  loop_tracker.execute();

  mpu9150.execute();
  if(mpu_reporting_ms && loop_tracker.every_n_ms(mpu_reporting_ms)) {
    Serial.print(loop_tracker.loop_count);
    Serial.print(" ");
    print_mpu_status();
  }

  // blink the LED
  digitalWrite(pin_led,micros()%1000000<5000);
}
