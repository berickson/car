#include "BNO055.h"

BNO055 mpu;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial) {};
  mpu.setTempSource(true);
  mpu.begin(POWER_MODE::NORMAL, OPERATION_MODE::FUSION_NDOF_NORMAL, PLACEMENT::P4, true); //P2
}
float data[3]{0.0,0.0,0.0};
bool calib = false;
void Calprint();
void HRPprint();
void accelprint();

void loop() {
  if (Serial.available() > 0) {
    char c = Serial.read();
    if (c=='s') mpu.saveCalib();
    if (c=='l') mpu.loadCalib();
    if (c=='c') Calprint();
    if (c=='e') HRPprint();
    if (c=='a') accelprint();
    if (c=='t') {
      Serial.print("Temperature: ");
      Serial.println(mpu.getTemp());
      }  
    }
    if (mpu.getCalibration() == 0xFF && !calib) {
      mpu.saveCalib();
      calib=true;
    }
    delay(500);
}

void Calprint() {
  int temp = mpu.getCalibration();
  Serial.println("             ToGyAcMa");
  Serial.print("Calibration: ");
  if (temp < 128) Serial.print("0");
  if (temp < 64) Serial.print("0");
  Serial.println(temp,BIN);
}

void HRPprint() {
  Serial.println("Eulers:");
  mpu.getEuler(data);
  Serial.print("Heading: ");
  Serial.println(data[0]);
  Serial.print("Roll: ");
  Serial.println(data[1]);
  Serial.print("Pitch: ");
  Serial.println(data[2]);
}

void accelprint() {
  Serial.print("Linear Acceleration X:");
  Serial.print(mpu.getLinearAccelX());
  Serial.print(" Y: ");
  Serial.print(mpu.getLinearAccelY());
  Serial.print(" Z: ");
  Serial.println(mpu.getLinearAccelZ());  
}

