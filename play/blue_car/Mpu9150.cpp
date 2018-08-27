#include "Mpu9150.h"

#define rads2degrees(radians) (radians * 180. / M_PI)


// ================================================================
// ===               INTERRUPT DETECTION ROUTINE                ===
// ================================================================

volatile bool interrupt_pending = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
    interrupt_pending = true;
}

// this will calibrate rest position based on gravity alone
void Mpu9150::calibrate_as_horizontal() {
    set_zero_orientation(qraw);
}


// based on http://stackoverflow.com/a/4436915/383967, modified
Quaternion quaternion_from_axis_angle(const float &xx, const float &yy, const float &zz, const float &a)
{
    // Here we calculate the sin( theta / 2) once for optimization
    float factor = sinf( a / 2.0 );

    // Calculate the x, y and z of the quaternion
    float x = xx * factor;
    float y = yy * factor;
    float z = zz * factor;

    // Calcualte the w value by cos( theta / 2 )
    float  w = cosf( a / 2.0 );
    Quaternion q = Quaternion(w, x, y, z);
    q.normalize();

    return q;
}

// this will calibrate forward based on titl and gravity
// use this to have pitch and yaw read correctly
// to calibrate:
// 1. Calibrate horizontal using calibrate_as_horizontal()
// lift front of vehicle off the ground, say 30 degrees or so
// then call this function.  The front of the vehicle will be calibrated
// based on the gravity vector.
void Mpu9150::calibrate_nose_up() {
    float theta = atan2f(gravity.x, gravity.y);
    Quaternion q = quaternion_from_axis_angle(0,0,1,theta + 3*PI/2);
    set_zero_orientation(q.getProduct(zero_adjust));
}

void Mpu9150::start_calibrate_at_rest(float pause_seconds, float test_seconds)
{
    Serial.println((String) "performing rest calibration.  Keep still for " + (pause_seconds + test_seconds) + " seconds");
    yaw_slope_rads_per_ms = 0; // reset yaw drift to properly measure
    at_rest_calibrating = true;
    at_rest_calibration_start_millis = millis() + 1000 * pause_seconds;
    at_rest_calibration_end_millis = at_rest_calibration_start_millis + 1000 * test_seconds;

    ax_stats.reset();
    ay_stats.reset();
    az_stats.reset();
    yaw_stats.reset();
}

void Mpu9150::set_zero_orientation(Quaternion zero) {
    log(LOG_INFO,"re-oriented zero on MPU");
    zero_adjust = zero;
}

void Mpu9150::enable_interrupts(int interrupt_pin) {
    // enable Arduino interrupt detection
    log(TRACE_MPU,F("Enabling interrupt detection (Arduino external interrupt "));
    pinMode(interrupt_pin, INPUT);
    attachInterrupt(interrupt_pin,dmpDataReady, RISING);
    interrupt_pending = mpu.getIntStatus();
    interrupt_enabled = true;
}

void Mpu9150::setup() {
    rest_a_mag = 7645.45;
    set_zero_orientation( Quaternion(-0.69, 0.000,0.73,-0.00) );
    ax_bias = 0.0;
    ay_bias = 0.0;
    az_bias = 0.0;

    yaw_adjust_start_ms = millis();
    yaw_raw_total = 0;

    // initialize device
    log(TRACE_MPU,"Initializing I2C MPU devices...");
    mpu.initialize();
    log(TRACE_MPU,"Done Initializing I2C MPU devices...");

    // verify connection
    if(mpu.testConnection()) {
        log(TRACE_MPU, "MPU9150 connection successful");
    } else {
        log(LOG_ERROR, "MPU9150 connection successful");
    }


    // load and configure the DMP
    log(TRACE_MPU,F("Initializing DMP..."));
    auto devStatus = mpu.dmpInitialize();

    // make sure it worked (returns 0 if so)
    if (devStatus == 0) {
        // turn on the DMP, now that it's ready
        log(TRACE_MPU,F("Enabling DMP..."));
        mpu.setDMPEnabled(true);
        log(TRACE_MPU,F("DMP ready"));

        // get expected DMP packet size for later comparison
        packetSize = mpu.dmpGetFIFOPacketSize();
        log(TRACE_MPU,(String) "packet size: "+packetSize);
    } else {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
        log(LOG_ERROR,F("DMP Initialization failed (code "));
        log(LOG_ERROR,devStatus);
        log(LOG_ERROR,F(")"));
    }

}

void Mpu9150::zero_heading() {
  this->yaw_raw_total = 0;
  this->yaw_adjust_start_ms = millis();
}


float standardized_degrees(float theta) {
  theta = fmod(theta, 360.0);
  if(theta < -180.)
      theta += 360.0;
    return theta;
}


// returns an inverted and corrected yaw value so rotation follows
// standard of ccw being positive
float Mpu9150::heading() {
    float yaw = (yaw_raw_total - (millis()-yaw_adjust_start_ms) * yaw_slope_rads_per_ms) * yaw_actual_per_raw;
    return standardized_degrees(-rads2degrees(yaw));
}

void Mpu9150::log_status() {
    log(TRACE_MPU, (String) ((int)readingCount) +
        ",heading,"+heading() +
        ",quat,"+ftos(q.w)+"," +ftos(q.x)+"," +ftos(q.y)+"," +ftos(q.z)+
        ",gravity,"+ftos(gravity.x)+","+ftos(gravity.y)+","+ftos(gravity.z)+
        ",araw,"+araw.x+","+araw.y+","+araw.z+
        ",aa,"+ftos(ax)+","+ftos(ay)+","+ftos(az)+
        ",mag,"+mag.x+","+mag.y+","+mag.z+
        ",ypr,"+heading()+","+ftos(rads2degrees(pitch))+","+ftos(rads2degrees(roll)) );
}

// returns radians from a1 to a2, considering wrap around
// (a2 - a1)
float radians_diff(float a2, float a1) {
  float d = a2-a1;
  if(d > M_PI)
    d -= 2 * M_PI;
  if(d < -M_PI)
    d += 2 * M_PI;

  return d;
}

void Mpu9150::execute(){
    // quickly return if there is nothing to do
    if(interrupt_enabled && !interrupt_pending) return;
    interrupt_pending = false;

    log(TRACE_MPU,(String) "reading fifo");
    fifoCount = mpu.getFIFOCount();
    log(TRACE_MPU,(String) "fifo count " + fifoCount + ", packet size " + packetSize);

    if (fifoCount < packetSize)
        return;

    if (fifoCount >= 1024) {
        mpu.resetFIFO();
        return;
    }

    while(fifoCount >= packetSize) {
        log(TRACE_MPU,"reading mpu");
        mpu.getFIFOBytes(fifoBuffer, packetSize);
        fifoCount -= packetSize;
        readingCount++;
    }

    log(TRACE_MPU, (String)"readingCount: " + readingCount);

    mpu.dmpGetQuaternion(&qraw, fifoBuffer);
    mpu.dmpGetGravity(&graw, &qraw);
    mpu.dmpGetAccel(&araw, fifoBuffer);
    a = araw.getRotated(&zero_adjust);

    q = qraw.getProduct(zero_adjust.getConjugate());
    gravity= graw.getRotated(&zero_adjust);

    const float g = 9.80665f;

    ax = g * (a.x/rest_a_mag - gravity.x);
    ay = g * (a.y/rest_a_mag - gravity.y);
    az = g * (a.z/rest_a_mag - gravity.z);

    temperature = mpu.getTemperature();

    float last_yaw = yaw_pitch_roll[0];
    mpu.dmpGetYawPitchRoll(yaw_pitch_roll, &q, &gravity);

    if(at_rest_calibrating) {
        auto ms = millis();
        if(ms >= at_rest_calibration_start_millis && ms < at_rest_calibration_end_millis) {
            ax_stats.add(ms,araw.x);
            ay_stats.add(ms,araw.y);
            az_stats.add(ms,araw.z);
            yaw_stats.add(ms,yaw_pitch_roll[0]); // todo: make sure this doesn't roll over in the middle of calibrating
        } else if (ms >= at_rest_calibration_end_millis ) {
            ax_bias = ax_stats.meany();
            ay_bias = ay_stats.meany();
            az_bias = az_stats.meany();
            rest_a_mag = sqrt(ax_bias*ax_bias + ay_bias * ay_bias + az_bias*az_bias);
            yaw_slope_rads_per_ms = yaw_stats.slope();
            yaw_adjust_start_ms = millis();

            Serial.println();
            Serial.print("stats collection completed, ");
            Serial.print(ax_stats.count);
            Serial.print(" samples ");
            Serial.print(" ax_bias: ");
            Serial.print(ax_bias);
            Serial.print(" ay_bias: ");
            Serial.print(ay_bias);
            Serial.print(" az_bias: ");
            Serial.print(az_bias);
            Serial.print(" rest_a_mag: ");
            Serial.print(rest_a_mag);
            Serial.print(" yaw_slope_rads_per_ms: ");
            Serial.print(yaw_slope_rads_per_ms,10);
            Serial.print(" degrees/hr drift: ");
            Serial.print(yaw_slope_rads_per_ms*1000*60*60*180/PI);

            Serial.println();


            // reset all the state variables
            at_rest_calibrating = false;
            at_rest_calibration_end_millis = 0;
            at_rest_calibration_start_millis = 0;
        }
    }
    float yaw_diff = radians_diff(yaw_pitch_roll[0], last_yaw);
    yaw_raw_total += yaw_diff;
    // cancel out the yaw drift
    //yaw_pitch_roll[0] -=   yaw_slope_rads_per_ms * (millis() - yaw_adjust_start_ms);

}

