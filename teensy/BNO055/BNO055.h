#pragma once
#include <Arduino.h>
#include <Wire.h>
#include "BNODEFS.h"

class BNO055 {
	uint8_t resetpin, addresspin;

	uint8_t SLADD = 0x28; /*slave address, dependent on address pin, low-28, high-29*/
	bool running = false; //internal variable for config state, true means NOT config
	bool page0 = true; //internal variable for page#
	bool writePage(uint8_t _page); //Select page to read/write from

	uint8_t meas_units, opmode;
	uint8_t init(POWER_MODE _Power, OPERATION_MODE _Config, PLACEMENT _orientation);

public:
#ifdef MyCalibrations
	bool printCalib(); //Print calibrations to serial monitor
	bool loadMyCal(); //Load calibrations from EEPROM or MyCal[]
#else
#ifdef EEPROM_h
	uint8_t eepromaddress = 0; //Storage for where to start write/read of data
	bool saveEEPROM(); //save calibrations to EEPROM
	bool loadEEPROM(); //load calibrations from EEPROM
#else
	bool printCalib(); //Print calibrations to serial monitor
#endif
#endif

	BNO055() { Wire.begin(); }
	//if orientation is > 7 you need to call orientation yourself before begin. Default orient is P1 otherwise.
	uint8_t begin(POWER_MODE _Power_Mode, OPERATION_MODE _Operation_Mode, PLACEMENT _orientation=PLACEMENT::P1, bool _Load=false, uint8_t _ResetPin=255, uint8_t _AddressPin=255, bool _SlaveHigh=false); 
	bool begin(); //simple begin(), true comms ok, false they aren't, setup needs to be done manually

	bool write(Page0 _register, uint8_t _data); //Write _data to _register, Page0
	bool write(Page1 _register, uint8_t _data); //Write _data to _register, Page1
	bool write(Page0 _register, uint8_t _NumBytes, uint8_t *_data); //Write _NumBytes of *_data starting at _register, Page0
	bool write(Page1 _register, uint8_t _NumBytes, uint8_t *_data); //Write _NumBytes of *_data starting at _register, Page1

	uint8_t read(Page0 _register); //Read a byte from _register, Page0
	uint8_t read(Page1 _register); //Read a byte from _register, Page1
	bool read(Page0 _register, uint8_t _NumBytes, uint8_t *_dataread); //Read _NumBytes from _register, store in *_dataread, Page0
	bool read(Page1 _register, uint8_t _NumBytes, uint8_t *_dataread); //Read _NumBytes from _register, store in *_dataread, Page1

	//Reads 16 bits at _register and _register+1, this is to ensure MSB and LSB reads are from same frame(for 16bit reads)
	int16_t read16(Page0 _register);

	//bool writePage(uint8_t _page); //Select page to read/write from PROTECTED
	//uint8_t readPage(); //read current page NOT NEEDED?

	//These set POWER_MODE not OPERATION_MODE
	inline bool suspend() { return write(Page0::POWER_MODE_REG, uint8_t(POWER_MODE::SUSPEND)); }//Suspend, everything halts
	inline bool resume() { return write(Page0::POWER_MODE_REG, uint8_t(POWER_MODE::NORMAL)); } //Sets to normal power mode, default mode
	inline bool sleep() { return write(Page0::POWER_MODE_REG, uint8_t(POWER_MODE::LOW_POWER)); } //Low power, uses wake on motion, interrupt
	bool reset(); //resets chip
	bool setOperationMode(OPERATION_MODE _Operation_Mode);//Sets OPERATION_MODE from config 7ms delay, to delay 19ms delay 
	bool setPowerMode(POWER_MODE _Power_Mode);//Sets power mode, TODO change for suspend, low pow?

	//Set measurement units for data outputs
	bool setUnits(uint8_t _UnitSel);
	//UNIT_SEL bits: 0,1,2,4,7
	bool setUnits(bool _Acceleration, //false is m/s�, true is mg
		bool _Angular_rate, //false is DPS, true is RPS
		bool _Euler_angles, //false is degrees true is radians
		bool _Temperature, //true is fahrenheit, false is centigrade
		bool _Fusion_Format);//True is android format, false is windows format
	uint8_t getUnits() { return read(Page0::UNIT_SELECTION); } //Returns UNIT_SELECTION
	/* UNIT_SELECTION
	Register Bits: 0[Acceleration], 1[Angular rate], 2[Euler Angles], 4[Temperature], 7[Fusion format]
	Acceleration: 0[m/s�], 1[mg]
	Angular Rate: 0[DPS], 1[RPS]
	Euler Angles: 0[Degrees], 1[Radians]
	Temperature: 0[Celsius], 1[Fahrenheit]
	Fusion data output format: 0[Windows], 1[Android]

	Windows pitch: (-)180 - 180 clockwise increase
	Android pitch: 180 - (-)180 clockwise decreas
	Roll: (-)90 - 90 increase with +inclination
	Heading/Yaw: 0-360* Clockwise increases
	*/

	inline bool isRunning() { return running; } //If available that means unit is NOT in config mode

	bool orientation(PLACEMENT _placement); //See datasheet for orientation options
	bool orientation(uint8_t _Axis_Config, uint8_t _Axis_Remap); //Manual orientation
	bool setAddress(bool _High);	//If true, slave high(0x29) else slave low(0x28)

	//Uses eeprom addresses eepromaddress+0-21
	bool saveCalib(); //Saves to eeprom if available, otherwise print to monitor
	bool loadCalib(); //Loads from MyCal if avail, otherwise EEPROM, if neither defined returns false

	uint8_t getCalibration() { return read(Page0::CALIBRATION_STATUS); } //Raw calibration byte
	uint8_t getOverallCalib() { return (getCalibration() & 0xC0) >> 6; } //System calib, returns 0-3
	uint8_t getGyroCalib() { return (getCalibration() & 0x30) >> 4; } //Gyro calib, returns 0-3
	uint8_t getAccCalib() {	return (getCalibration() & 0x0C) >> 2; } //Acc calib, returns 0-3
	uint8_t getMagCalib() { return (getCalibration() & 0x03); } //Mag calib, returns 0-3

	bool setTempSource(bool Gyro); //true is gyro, false is accel
	bool getTempSource(); //true is Gyro, false is accel
	int8_t getTemp(); //returns temp in C or F, depending on UNIT_SELECTION

	//Return data based off UNIT_SELECTION, not all opmodes populate registers
	bool getEuler(float *_data); //Format is Heading, Roll, Pitch
	float getEulerPitch(); //returns pitch in euler angles(degrees or radians)
	float getEulerHeading(); //returns heading in euler angles(degrees or radians)
	float getEulerRoll(); // Returns roll in euler angles(degrees or radians)

	bool getGravity(float *_data); //Format is X, Y, Z 
	float getGravityVectorZ(); //returns gravity vector(m/s� or mg)
	float getGravityVectorY(); //returns gravity vector(m/s� or mg)
	float getGravityVectorX(); //returns gravity vector(m/s� or mg)

	bool getLinearAccel(float *_data); //Format is X, Y, Z 
	float getLinearAccelZ(); //Returns linear accel(m/s� or mg)
	float getLinearAccelY(); //Returns linear accel(m/s� or mg)
	float getLinearAccelX(); //Returns linear accel(m/s� or mg)

	bool getQuaternion(double *_data); //Format is W, X, Y, Z 
	double getQuatZ(); //Returns quat Z part /16384?
	double getQuatY(); //Returns quat Y part
	double getQuatX(); //Returns quat X part
	double getQuatW(); //Returns quat W part

	//data in specified unit(UNIT_SELECTION)
	bool getGyro(float *_data); //Format is X, Y, Z 
	float getGyroZ(); //Returns gyro Z reading(DPS or RPS)
	float getGyroY(); //Returns gyro Y reading(DPS or RPS)
	float getGyroX(); //Returns gyro X reading(DPS or RPS)

	bool getAccel(float *_data); //Format is X, Y, Z 
	float getAccelZ(); //Returns acc Z reading(m/s� or mg)
	float getAccelY(); //Returns acc Y reading(m/s� or mg)
	float getAccelX(); //Returns acc X reading(m/s� or mg)

	bool getMag(float *_data); //Format is X, Y, Z 
	float getMagZ(); //Returns mag Z reading in uT
	float getMagY(); //Returns mag Y reading in uT
	float getMagX(); //Returns mag X reading in uT

	//All page1 registers
	//Auxiliary functions used for manual sensor config and interrupt setup, see datasheet for info
	bool setMagConfig(uint8_t _Mag_Config);
	uint8_t getMagConfig();

	bool setGyroConfig(uint8_t _Config0, uint8_t _Config1);
	uint8_t getGyroConfig0();
	uint8_t getGyroConfig1();
	bool setGyroSleepConfig(uint8_t _SleepData); 
	uint8_t getGyroSleepConfig(); 
	bool setGyroInterruptSettings(uint8_t _GyroIntSettings); 
	uint8_t getGyroInterruptSettings(); 
	bool setGyroHR_XThreshold(uint8_t _HighRateX);
	uint8_t getGyroHR_XThreshold();
	bool setGyroHR_XDuration(uint8_t _HighRateXDur);
	uint8_t getGyroHR_XDuration();
	bool setGyroHR_YThreshold(uint8_t _HighRateY);
	uint8_t getGyroHR_YThreshold();
	bool setGyroHR_YDuration(uint8_t _HighRateYDur);
	uint8_t getGyroHR_YDuration();
	bool setGyroHR_ZThreshold(uint8_t _HighRateZ);
	uint8_t getGyroHR_ZThreshold();
	bool setGyroHR_ZDuration(uint8_t _HighRateZDur);
	uint8_t getGyroHR_ZDuration();
	bool setGyroAnyMotionThreshold(uint8_t _AMThresh);
	uint8_t getGyroAnyMotionThreshold();
	bool setGyroAnyMotionSettings(uint8_t _AMSettings);
	uint8_t getGyroAnyMotionSettings();

	bool setAccConfig(uint8_t ACC_Config); //raw byte
	bool setAccConfig(ACC_RANGE _G_Range, ACC_BANDWIDTH _Bandwidth, ACC_OPMODE _Op_Mode); //use enums
	uint8_t getAccConfig();
	bool setAccSleepConfig(uint8_t _SleepData);
	uint8_t getAccSleepConfig();
	bool setAccAnyMotionThreshold(uint8_t _AMTheshold);
	uint8_t getAccAnyMotionThreshold(); 
	bool setAccInterruptSettings(uint8_t _IntSettings);
	uint8_t getAccInterruptSettings();
	bool setAccHighGDuration(uint8_t _HighGDuration);
	uint8_t getAccHighGDuration();
	bool setAccHighGThreshold(uint8_t _HighGThreshold);
	uint8_t getAccHighGThreshold();
	bool setAccNoMotionThreshold(uint8_t _NoMotionThreshold);
	uint8_t getAccNoMotionThreshold();
	bool setAccNoMotionSetting(uint8_t _NoMotionSet);
	uint8_t getAccNoMotionSetting();

	bool setInterruptEnable(uint8_t _IntEnable);
	uint8_t getInterruptEnable();
	bool setInterruptMask(uint8_t _IntMask); //Interrupt mask allows INT pin to toggle, otherwise just updates Page0::INTERRUPT_STATUS register
	uint8_t getInterruptMask();

#ifdef EEPROM_h
	void setEepromAddress(uint8_t _address) { if (_address <= (EEPROM.length() - 23)) eepromaddress = _address; } //Set 22 byte EEPROM starting address
	uint8_t getEepromAddress() { return eepromaddress; } //Get 22 byte EEPROM starting address
#endif
};
