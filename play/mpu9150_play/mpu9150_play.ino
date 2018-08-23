// all these ugly pushes are becaues the 9150 has a lot of warnings
// the .h file must be included in one time in a source file
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-value"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#include <MPU9150_9Axis_MotionApps41.h>
#pragma GCC diagnostic pop

const uint16_t pin_mpu_interrupt = 17;


#include "Mpu9150.h"

Mpu9150 mpu9150;

void setup() {
    Serial.println("starting wire");
    Wire.begin();

    Serial.println("starting mpu");

    mpu9150.enable_interrupts(pin_mpu_interrupt);
    log(LOG_INFO, "Interrupts enabled for mpu9150");
    mpu9150.setup();
    log(LOG_INFO, "MPU9150 setup complete");
    mpu9150.ax_bias = 0; // 7724.52;
    mpu9150.ay_bias = 0; // -1458.47;
    mpu9150.az_bias = 0; // 715.62;
    mpu9150.rest_a_mag = 0; // 7893.51;
    mpu9150.zero_adjust = Quaternion(0.0, 0.0, 0.0, 1);// Quaternion(-0.07, 0.67, -0.07, 0.73);
    // was ((-13.823402+4.9) / (1000 * 60 * 60)) * PI/180;
    mpu9150.yaw_slope_rads_per_ms  = (2.7 / (10 * 60 * 1000)) * PI / 180;
    mpu9150.yaw_actual_per_raw = 1; //(3600. / (3600 - 29.0 )); //1.0; // (360.*10.)/(360.*10.-328);// 1.00; // 1.004826221;

    mpu9150.zero_heading();
    Serial.println("mpu setup complete");
}

unsigned long loop_time_ms = 0;
unsigned long last_loop_time_ms = 0;

// returns true if loop time passes through n ms boundary
bool every_n_ms(unsigned long last_loop_ms, unsigned long loop_ms, unsigned long ms) {
  return (last_loop_ms % ms) + (loop_ms - last_loop_ms) >= ms;
}

void loop() {

    last_loop_time_ms = loop_time_ms;
    loop_time_ms = millis();

    mpu9150.execute();
    if (every_n_ms(last_loop_time_ms, loop_time_ms, 500)) {
        Serial.println((String)"heading: " + mpu9150.heading());

    }

}
