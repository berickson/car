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

void Mpu9150::set_zero_orientation(Quaternion zero)
{
  Serial.println("Mpu9150::set_zero_orientation");
  zero_adjust = zero;
}

void Mpu9150::setup() {

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
  return -rads2degrees(yaw_pitch_roll[0]);
}

void Mpu9150::log_status() {
  log(TRACE_MPU, (String)readingCount + 
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

    // otherwise, check for DMP data ready interrupt (this should happen frequently)
  } else { //if (mpuIntStatus & 0x01) {
    // wait for correct available data length, should be a VERY short wait
    log(TRACE_MPU,"reading mpu");
    // read a packet from FIFO
    mpu.getFIFOBytes(fifoBuffer, packetSize);
    fifoCount -= packetSize;
    
    mpu.dmpGetQuaternion(&qraw, fifoBuffer);
    mpu.dmpGetGravity(&graw, &qraw);
    mpu.dmpGetAccel(&araw, fifoBuffer);
    VectorInt16 alinraw;
    mpu.dmpGetLinearAccel(&alinraw, &araw, &graw);
    a = alinraw.getRotated(&zero_adjust);

    ax = a.x;
    ay = a.y;
    az = a.z;

    q = qraw.getProduct(zero_adjust.getConjugate());
    gravity= graw.getRotated(&zero_adjust);

    mpu.dmpGetYawPitchRoll(yaw_pitch_roll, &q, &gravity);


    readingCount++;

    // get initial quaternion and gravity
    if(!initialReading) {
      zero();
      initialReading = true;
    }
  }
}

void Mpu9150::zero() {
  mpu.dmpGetAccel(&a0, fifoBuffer);
}
