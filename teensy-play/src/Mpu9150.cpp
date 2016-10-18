#include "Mpu9150.h"

#define rads2degrees(radians) (radians * 180. / M_PI)

// ====== double2s(): print out up to 16 digits of input double-precision value
// This version enables double-precision for Teensy 3, etc. 
// by J.Beale March 2013
// modified from original float2s() posted by davekw7x on December, 2010
// http://arduino.cc/forum/index.php/topic,46931.0.html



// ================================================================
// ===               INTERRUPT DETECTION ROUTINE                ===
// ================================================================

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
  mpuInterrupt = true;
}

// this will calibrate rest position based on gravity alone
void Mpu9150::calibrate_as_horizontal() {
  set_zero_orientation(qraw);
}


// based on http://stackoverflow.com/a/4436915/383967, modified
Quaternion quaternion_from_axis_angle(const double &xx, const double &yy, const double &zz, const double &a)
{
    // Here we calculate the sin( theta / 2) once for optimization
    double factor = sin( a / 2.0 );

    // Calculate the x, y and z of the quaternion
    double x = xx * factor;
    double y = yy * factor;
    double z = zz * factor;

    // Calcualte the w value by cos( theta / 2 )
    double w = cos( a / 2.0 );
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
  double theta = atan2(gravity.x, gravity.y);
  Quaternion q = quaternion_from_axis_angle(0,0,1,theta + 3*PI/2);
  set_zero_orientation(q.getProduct(zero_adjust));
}

void Mpu9150::start_calibrate_at_rest(double pause_seconds, double test_seconds)
{
  Serial.println((String) "performing rest calibration.  Keep still for " + test_seconds + " seconds");
  at_rest_calibrating = true;
  at_rest_calibration_start_millis = millis() + 1000 * pause_seconds;
  at_rest_calibration_end_millis = at_rest_calibration_start_millis + 1000 * test_seconds;

  ax_stats.reset();
  ay_stats.reset();
  az_stats.reset();
  yaw_stats.reset();
}

void Mpu9150::set_zero_orientation(Quaternion zero) {
  Serial.println("re-oriented zero on MPU");
  zero_adjust = zero;
}

void Mpu9150::setup() {
  rest_a_mag = 7645.45;
  set_zero_orientation( Quaternion(-0.69, 0.000,0.73,-0.00) );
  ax_bias = 0.0;
  ay_bias = 0.0;
  az_bias = 0.0;

  // initialize device
  Serial.println("9150 setup");
  log(TRACE_MPU,"Initializing I2C MPU devices...");
  mpu.initialize();
  log(TRACE_MPU,"Done Initializing I2C MPU devices...");

  // verify connection
  if(mpu.testConnection()) {
    log(TRACE_MPU, "MPU9150 connection successful")
  } else {
    log(LOG_ERROR, "MPU9150 connection successful")
  }


  // load and configure the DMP
  log(TRACE_MPU,F("Initializing DMP..."));
  devStatus = mpu.dmpInitialize();

  // make sure it worked (returns 0 if so)
  if (devStatus == 0) {
    // turn on the DMP, now that it's ready
    log(TRACE_MPU,F("Enabling DMP..."));
    mpu.setDMPEnabled(true);

    // enable Arduino interrupt detection
    log(TRACE_MPU,F("Enabling interrupt detection (Arduino external interrupt ")); Serial.println(INTERRUPT_NUMBER);
    pinMode(INTERRUPT_PIN, INPUT);
    attachInterrupt(INTERRUPT_NUMBER,dmpDataReady, RISING);
    mpuIntStatus = mpu.getIntStatus();

    // set our DMP Ready flag so the main loop() function knows it's okay to use it
    Serial.println(F("DMP ready!"));
    dmpReady = true;

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

// returns an inverted yaw value so rotation follows
// standard of ccw being positive
float Mpu9150::heading() {
  if(!initialReading) return 0.0;

  // cancel out the yaw drift
  // todo: is there a better place to do this?
  float yaw = yaw_pitch_roll[0];
  yaw -=   yaw_slope_rads_per_ms * (millis() - yaw_adjust_start_ms);
  return -rads2degrees(yaw);
}

void Mpu9150::log_status() {
  log(TRACE_MPU, (String) ((int)readingCount) +
    ",heading,"+heading() +
    ",quat,"+ftos(q.w)+"," +ftos(q.x)+"," +ftos(q.y)+"," +ftos(q.z)+
    ",gravity,"+ftos(gravity.x)+","+ftos(gravity.y)+","+ftos(gravity.z)+
    ",araw,"+araw.x+","+araw.y+","+araw.z+
    ",aa,"+ftos(ax)+","+ftos(ay)+","+ftos(az)+
    ",mag,"+mag.x+","+mag.y+","+mag.z+
    ",ypr,"+ftos(rads2degrees(yaw))+","+ftos(rads2degrees(pitch))+","+ftos(rads2degrees(roll)) );
}

void Mpu9150::execute(){
  log(TRACE_MPU,(String) "reading fifo");
  fifoCount = mpu.getFIFOCount();
  // wait for MPU interrupt or extra packet(s) available
  log(TRACE_MPU,(String) "fifo count " + fifoCount);

  if (fifoCount < packetSize)
  return;

  // reset interrupt flag and get INT_STATUS byte
  mpuInterrupt = false;
  mpuIntStatus = mpu.getIntStatus();

  if (fifoCount >= 1024) {
    mpu.resetFIFO();
    Serial.println(F("FIFO overflow!"));

  } else { //if (mpuIntStatus & 0x01) {
    // wait for correct available data length, should be a VERY short wait
    log(TRACE_MPU,"reading mpu");
    // read a packet from FIFO
    mpu.getFIFOBytes(fifoBuffer, packetSize);
    fifoCount -= packetSize;
    
    mpu.dmpGetQuaternion(&qraw, fifoBuffer);
    mpu.dmpGetGravity(&graw, &qraw);
    mpu.dmpGetAccel(&araw, fifoBuffer);
    a = araw.getRotated(&zero_adjust);



    q = qraw.getProduct(zero_adjust.getConjugate());
    gravity= graw.getRotated(&zero_adjust);

    ax = 9.8*(a.x/rest_a_mag - gravity.x);
    ay = 9.8*(a.y/rest_a_mag - gravity.y);
    az = 9.8*(a.z/rest_a_mag - gravity.z);


    mpu.dmpGetYawPitchRoll(yaw_pitch_roll, &q, &gravity);


    readingCount++;

    // get initial quaternion and gravity
    if(!initialReading) {
      zero();
      initialReading = true;
    }
    if(at_rest_calibrating) {
      auto ms = millis();
      if(ms >= at_rest_calibration_start_millis && ms < at_rest_calibration_end_millis) {
        ax_stats.add(ms,araw.x);
        ay_stats.add(ms,araw.y);
        az_stats.add(ms,araw.z);
        yaw_stats.add(ms,yaw); // todo: make sure this doesn't roll over in the middle of calibrating
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
        Serial.print("degrees/hr drift: ");
        Serial.print(yaw_slope_rads_per_ms*1000*60*60*180/PI);

        Serial.println();


        // reset all the state variables
        at_rest_calibrating = false;
        at_rest_calibration_end_millis = 0;
        at_rest_calibration_start_millis = 0;
      }
    }
  }
}

void Mpu9150::zero() {
  mpu.dmpGetAccel(&a0, fifoBuffer);
}
