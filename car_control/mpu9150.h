
#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050_9Axis_MotionApps41.h"
#include "vector_math.h"


MPU6050 mpu;

#define INTERRUPT_PIN 12
#define INTERRUPT_NUMBER 12



// ================================================================
// ===               INTERRUPT DETECTION ROUTINE                ===
// ================================================================

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
  mpuInterrupt = true;
}

class Mpu9150 {
public:

  // MPU control/status vars
  bool dmpReady = false;  // set true if DMP init was successful
  bool initialReading = false; // set to true if we have initial reading
  uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
  uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
  uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
  uint16_t fifoCount;     // count of all bytes currently in FIFO
  uint8_t fifoBuffer[64]; // FIFO storage buffer
  unsigned long readingCount;

  // orientation/motion vars
  Quaternion q;           // [w, x, y, z]         quaternion container
  Quaternion q0;          // initial quaternion
  VectorInt16 aa;         // [x, y, z]            accel sensor measurements
  VectorInt16 a0;
  VectorInt16 mag;
  VectorFloat gravity;    // [x, y, z]            gravity vector



  // ================================================================
  // ===                      INITIAL SETUP                       ===
  // ================================================================

  void setup_mpu() {
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

  double ground_angle() {
    if(!initialReading) return 0.0;

    Quaternion qdiff = diff(q,q0);
    double angle = rads2degrees(normal_angle(qdiff,gravity));
    return angle;
  }


  void trace_mpu() {

    Serial.print(readingCount);
    Serial.print("\t");
    Serial.print("angle");
    Serial.print("\t");
    Serial.print(ground_angle());
    Serial.print("\t");
    Serial.print("quat\t");
    Serial.print(q.w);
    Serial.print("\t");
    Serial.print(q.x);
    Serial.print("\t");
    Serial.print(q.y);
    Serial.print("\t");
    Serial.print(q.z);
    Serial.print("\t");
    Serial.print("gravity\t");
    Serial.print(gravity.x);
    Serial.print("\t");
    Serial.print(gravity.y);
    Serial.print("\t");
    Serial.print(gravity.z);
    Serial.print("\t");
    Serial.print("aa\t");
    Serial.print(aa.x);
    Serial.print("\t");
    Serial.print(aa.y);
    Serial.print("\t");
    Serial.print(aa.z);
    Serial.print("\t");
    Serial.print("mag\t");
    Serial.print(mag.x);
    Serial.print("\t");
    Serial.print(mag.y);
    Serial.print("\t");
    Serial.print(mag.z);
    Serial.println();
  }


  inline void process_mpu(){
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

      mpu.dmpGetQuaternion(&q, fifoBuffer);
      mpu.dmpGetGravity(&gravity, &q);
      mpu.dmpGetMag(&mag, fifoBuffer);
      mpu.dmpGetAccel(&aa, fifoBuffer);
      readingCount++;

      // get initial quaternion and gravity
      if(!initialReading) {
        zero();
        initialReading = true;
      }
    }
  }


  void setup(){
    setup_mpu();
  }
  inline void execute(){
    process_mpu();
  }
  void trace_status() {
    trace_mpu();
  }
  void zero() {
    mpu.dmpGetAccel(&a0, fifoBuffer);
  }
};
