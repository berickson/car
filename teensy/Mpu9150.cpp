#include "Mpu9150.h"

#include "helper_3dmath.h"
#include <MPU9150_9Axis_MotionApps41.h>


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

  // check for overflow (this should never happen unless our code is too inefficient)
  if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
    // reset so we can continue cleanly
    mpu.resetFIFO();
    Serial.println(F("FIFO overflow!"));

    // otherwise, check for DMP data ready interrupt (this should happen frequently)
  } else { //if (mpuIntStatus & 0x01) {

    // read a packet from FIFO
    log(TRACE_MPU,"reading mpu");
    mpu.getFIFOBytes(fifoBuffer, packetSize);
    fifoCount -= packetSize;
    
    mpu.dmpGetQuaternion(&q, fifoBuffer);

    const float g =  9.80665;

    
    ax = araw.x;
    ay = araw.y;
    az = araw.z;

#if 1
    // the following adjusts for the orientation of the mpu mounted in the car
    // while sitting flat on the ground
    // original measurement was (0.674316,-0.0788574,-0.731384,0.0640259)
    // below is the conjugate which will set it to unity
    Quaternion adjust = Quaternion(0.674316,0.0788574,0.731384,-0.0640259);
    q = q.getProduct(adjust);
    ax = g*(araw.x * 0.0000144756 + araw.y * -0.0000070556+ araw.z* 0.0001214348 - 0.043 + gravity.x);
    ay = g*(araw.x * -0.0000203849+ araw.y * -0.0001212882+ araw.z* 0.0000038742 -0.032  + gravity.y);
    az = g*(araw.x * -0.0001197524+ araw.y *0.0000187399 + araw.z*0.0000192036 + -0.053  + gravity.z);
    Serial.println((String) "araw:[" + araw.x + "," + araw.y + "," + araw.z + "]");
    VectorInt16 arot(araw.x,araw.y,araw.z);
    arot.rotate(&adjust);
    Serial.println((String) "arot:[" + arot.x + "," + arot.y + "," + arot.z + "]");

#endif
    mpu.dmpGetGravity(&gravity, &q);

    mpu.dmpGetMag((int16_t *) &mag, fifoBuffer);
    mpu.dmpGetAccel(&araw, fifoBuffer);


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
