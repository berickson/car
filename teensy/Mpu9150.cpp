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

void Mpu9150::setup() {
  Wire.begin();
  //Serial.begin(115200);

  // initialize device
  Serial.println(F("Initializing I2C MPU devices..."));
  mpu.initialize();

  // verify connection
  Serial.println(F("Testing device connections..."));
  Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));


  // load and configure the DMP
  Serial.println(F("Initializing DMP..."));
  devStatus = mpu.dmpInitialize();

  // make sure it worked (returns 0 if so)
  if (devStatus == 0) {
    // turn on the DMP, now that it's ready
    Serial.println(F("Enabling DMP..."));
    mpu.setDMPEnabled(true);

    // enable Arduino interrupt detection
    Serial.print(F("Enabling interrupt detection (Arduino external interrupt ")); Serial.println(INTERRUPT_NUMBER);
    pinMode(INTERRUPT_PIN, INPUT);
    attachInterrupt(INTERRUPT_NUMBER,dmpDataReady, RISING);
    mpuIntStatus = mpu.getIntStatus();

    // set our DMP Ready flag so the main loop() function knows it's okay to use it
    Serial.println(F("DMP ready! Waiting for first interrupt..."));
    dmpReady = true;

    // get expected DMP packet size for later comparison
    packetSize = mpu.dmpGetFIFOPacketSize();
  } else {
    // ERROR!
    // 1 = initial memory load failed
    // 2 = DMP configuration updates failed
    // (if it's going to break, usually the code will be 1)
    Serial.print(F("DMP Initialization failed (code "));
    Serial.print(devStatus);
    Serial.println(F(")"));
  }

}

double Mpu9150::ground_angle() {
  if(!initialReading) return 0.0;
  return rads2degrees(yaw_pitch_roll[0]);
}

String ftos(float f) {
  char buffer[80];
  sprintf(buffer,"%4g",f);
  return buffer;
}

void Mpu9150::log_status() {
  log(TRACE_MPU, (String)readingCount + 
    ",angle,"+ground_angle() +
    ",quat,"+ftos(q.w)+"," +ftos(q.x)+"," +ftos(q.y)+"," +ftos(q.z)+
    ",gravity,"+ftos(gravity.x)+","+ftos(gravity.y)+","+ftos(gravity.z)+
    ",aa,"+aa.x+","+aa.y+","+aa.z+
    ",mag,"+mag.x+","+mag.y+","+mag.z+
    ",ypr,"+ftos(rads2degrees(yaw))+","+ftos(rads2degrees(pitch))+","+ftos(rads2degrees(roll)) );
}


Quaternion qrotate(Quaternion q, Quaternion r) {
  auto rc = r.getConjugate();
  auto rq = r.getProduct(q);
  auto rv = rq.getProduct(rc);
  return rv;
}

void Mpu9150::execute(){
  // wait for MPU interrupt or extra packet(s) available
  if (!mpuInterrupt && fifoCount < packetSize)
  return;

  // reset interrupt flag and get INT_STATUS byte
  mpuInterrupt = false;
  mpuIntStatus = mpu.getIntStatus();

  // get current FIFO count
  fifoCount = mpu.getFIFOCount();

  // check for overflow (this should never happen unless our code is too inefficient)
  if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
    // reset so we can continue cleanly
    mpu.resetFIFO();
    Serial.println(F("FIFO overflow!"));

    // otherwise, check for DMP data ready interrupt (this should happen frequently)
  } else if (mpuIntStatus & 0x01) {
    // wait for correct available data length, should be a VERY short wait
    while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();

    // read a packet from FIFO
    mpu.getFIFOBytes(fifoBuffer, packetSize);

    // track FIFO count here in case there is > 1 packet available
    // (this lets us immediately read more without waiting for an interrupt)
    fifoCount -= packetSize;
    
<<<<<<< Updated upstream
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    
    auto rotate_x = Quaternion(0.7071067811865475,0.7071067811865475,0,0);
    auto rotate_y = Quaternion(0.7071067811865475,0,0.7071067811865475,0);
    auto rotate_z = Quaternion(0.7071067811865475,0,0,0.7071067811865475);
    
    Quaternion adjust = Quaternion(0.674316,0.0788574,0.731384,-0.0640259);
    q = q.getProduct(adjust);
    
    
=======
    Quaternion q_raw;
    mpu.dmpGetQuaternion(&q_raw, fifoBuffer);
    Quaternion rotate_y(-0.7071,0,0.7071,0); // rotate y 90 degrees
    Quaternion rotate_z(.995396,-0.09584,0,0); // rotate z 11 degrees (roll from sloped car edge)
    rotate_y.normalize();
    rotate_z.normalize();
    // rotate because board is mounted tilted: todo, handle slight tilt of other axis
    Quaternion q = q_raw.getProduct(rotate_z).getProduct(rotate_y);
>>>>>>> Stashed changes
    mpu.dmpGetGravity(&gravity, &q);
    
    mpu.dmpGetMag(&mag, fifoBuffer);
    mpu.dmpGetAccel(&aa, fifoBuffer);
    
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
