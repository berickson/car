#include "BNO055.h"

uint8_t BNO055::init(POWER_MODE _Power, OPERATION_MODE _Operation_Mode, PLACEMENT _orientation)
{
	uint8_t ret = 0;

	write(Page0::SYSTEM_TRIGGER, 0x01); //External oscillator
	write(Page0::UNIT_SELECTION, 0x90); //Android orientation(pitch+ counter clockwise), fahrenheit temp, euler degrees, rotation DPS, accel m/s�
	meas_units = 0x90;
	orientation(_orientation);
	write(Page0::POWER_MODE_REG, uint8_t(_Power));
	write(Page0::OPERATION_MODE_REG, uint8_t(_Operation_Mode));

	if (_Operation_Mode != OPERATION_MODE::CONFIG) delay(7); //wait 7ms to come out of config mode
	return ret;
}
//Barebones begin, just checks that self test/communication are ok. All setup is external to this
bool BNO055::begin()
{
	uint8_t counter = 0;
	if (!page0) writePage(0);
	while (read(Page0::SELFTEST_RESULT) != 0x0F) {
		delay(25);
		counter++;
		if (counter == 100) return false;
	}
	meas_units = 0x80;
	opmode = 0x00;

	return true;
}
uint8_t BNO055::begin(POWER_MODE _Power_Mode, OPERATION_MODE _Operation_Mode, PLACEMENT _orientation, bool _Load, uint8_t _ResetPin, uint8_t _AddressPin, bool _SlaveHigh)
{
	if (running) return 0xFF; //if it's already running you can't do this

	uint8_t counter, ret;
	counter = ret = 0;
	resetpin = _ResetPin;
	addresspin = _AddressPin;

	if (resetpin != 255) {
		pinMode(resetpin, OUTPUT);
		digitalWrite(resetpin, HIGH);
	}
	if (addresspin != 255) {
		pinMode(addresspin, OUTPUT);
		setAddress(_SlaveHigh);
	}
	else SLADD = 0x28;

	//If self test hasn't returned all success wait... may still be booting
	while (read(Page0::SELFTEST_RESULT) != 0x0F) {
		delay(25);  //We delay to keep chip from locking up in case of some error
		counter++;
		if (counter == 100) return 0xFF; //Doesn't seem like its going to happen return FF
	}

	if (_Load) loadCalib();

	//Power or config mode invalid, return with high byte flagged
	//if (_Power_Mode > 2 || _Operation_Mode > 12) return 0x80;

	ret |= init(_Power_Mode, _Operation_Mode, _orientation);
	opmode = uint8_t(_Operation_Mode);
	if (_Operation_Mode != OPERATION_MODE::CONFIG) running = true; //BNO055 is ready and available(configmode is NOT selected)

	return ret;
}

//returns 0 on error, sadly any value could be ok
uint8_t BNO055::read(Page0 _register)
{
	if (!page0) writePage(0);

	Wire.beginTransmission(SLADD);
	Wire.write(uint8_t(_register));
	Wire.endTransmission();

	if (Wire.requestFrom(SLADD, uint8_t(1))) return Wire.read();
	return 0;
}

//returns 0 on error, sadly any value could be ok
uint8_t BNO055::read(Page1 _register)
{
	if (page0) writePage(1);

	Wire.beginTransmission(SLADD);
	Wire.write(uint8_t(_register));
	Wire.endTransmission();

	if (Wire.requestFrom(SLADD, uint8_t(1))) return Wire.read();
	return 0;
}
/*
uint8_t BNO055::read(uint8_t _register, uint8_t _page)
{
	uint8_t ret = 0;

	if (_page == page0) {
		writePage(_page);
		page0 = !page0;
	}

	Wire.beginTransmission(SLADD);
	Wire.write(_register);
	Wire.endTransmission();

	if (Wire.requestFrom(SLADD, uint8_t(1))) ret = Wire.read();
	if (page0 == _page) writePage(!_page);

	return ret;
}*/

bool BNO055::read(Page0 _register, uint8_t _NumBytes, uint8_t *_dataread)
{
	if (!page0) writePage(0);

	if (_NumBytes == 0) return false;
	if (_NumBytes < 2) {
		_dataread[0] = read(_register);
		return true;
	}

	Wire.beginTransmission(SLADD);
	Wire.write(uint8_t(_register));
	Wire.endTransmission();

	if (Wire.requestFrom(SLADD, _NumBytes) != _NumBytes) return false;

	if (Wire.available() >= _NumBytes) {
		for (size_t i = 0; i < _NumBytes; ++i) _dataread[i] = Wire.read();
		return true;
	}
	return false;
}

bool BNO055::read(Page1 _register, uint8_t _NumBytes, uint8_t *_dataread)
{
	if (page0) writePage(1);

	if (_NumBytes == 0) return false;
	if (_NumBytes < 2) {
		_dataread[0] = read(_register);
		return true;
	}

	Wire.beginTransmission(SLADD);
	Wire.write(uint8_t(_register));
	Wire.endTransmission();

	if (Wire.requestFrom(SLADD, _NumBytes) != _NumBytes) return false;

	if (Wire.available() >= _NumBytes) {
		for (size_t i = 0; i < _NumBytes; ++i) _dataread[i] = Wire.read();
		return true;
	}
	return false;
}

int16_t BNO055::read16(Page0 _register)
{
	if (!page0) writePage(0);

	Wire.beginTransmission(SLADD);
	Wire.write(uint8_t(_register));
	Wire.endTransmission();

	Wire.requestFrom(SLADD, uint8_t(2));

	uint8_t low, high;

	low = Wire.read();
	high = Wire.read();

	return (high << 8) | low;
}

bool BNO055::write(Page0 _register, uint8_t _data)
{
	if (!page0) writePage(0);

	if (running && _register != Page0::OPERATION_MODE_REG) return false;

	uint8_t ret;

	//Record any change to units of measure
	if (_register == Page0::UNIT_SELECTION) meas_units = _data;

	Wire.beginTransmission(SLADD);
	Wire.write(uint8_t(_register));
	Wire.write(_data);
	ret = Wire.endTransmission();

	//Min delay between writes is 2 micros in normal, 450micros in low_power or suspend
	//Shouldn't be an issue since writes are for setup
	if (opmode == uint8_t(POWER_MODE::NORMAL)) delayMicroseconds(2);
	else delayMicroseconds(450);

	if (ret == 0) return true;
	else return false;
}

bool BNO055::write(Page1 _register, uint8_t _data)
{
	if (page0) writePage(1);

	if (running && (_register != Page1::INTERRUPT_MASK || _register != Page1::INTERRUPT_ENABLE)) return false;

	uint8_t ret;

	Wire.beginTransmission(SLADD);
	Wire.write(uint8_t(_register));
	Wire.write(_data);
	ret = Wire.endTransmission();

	if (ret == 0) return true;
	else return false;
}

bool BNO055::write(Page0 _register, uint8_t _NumBytes, uint8_t *_data)
{
	if (!page0) writePage(0);

	if (_NumBytes == 0) return false;
	if (_NumBytes < 2) return write(_register, _data[0]);

	if (running && _register != Page0::OPERATION_MODE_REG) return false;

	bool ret;

	for (size_t i = 0; i < _NumBytes; ++i)
	{
		//Record any change to units of measure
		if (Page0(_register + i) == Page0::UNIT_SELECTION) meas_units = _data[i];

		ret = write(Page0(uint8_t(_register) + i), _data[i]);
		//Min delay between write is 2 micros in normal, 450micros in low_power or suspend
		//Shouldn't be an issue since writes are for setup
		if (opmode == uint8_t(POWER_MODE::NORMAL)) delayMicroseconds(2);
		else delayMicroseconds(450);

		if (!ret) return false;
	}
	return true;
}

bool BNO055::write(Page1 _register, uint8_t _NumBytes, uint8_t *_data)
{
	if (page0) writePage(1);

	if (_NumBytes == 0) return false;
	if (_NumBytes < 2) return write(_register, _data[0]);

	if (running && (_register != Page1::INTERRUPT_ENABLE || _register != Page1::INTERRUPT_MASK)) return false;

	bool ret;

	for (size_t i = 0; i < _NumBytes; ++i)
	{
		ret = write(Page1(uint8_t(_register) + i), _data[i]);
		//Min delay between write is 2 micros in normal, 450micros in low_power or suspend
		//Shouldn't be an issue since writes are for setup
		if (opmode == uint8_t(POWER_MODE::NORMAL)) delayMicroseconds(2);
		else delayMicroseconds(450);

		if (!ret) return false;
	}
	return true;
}

/*
bool BNO055::write(uint8_t _register, uint8_t _data, bool _page)
{
	//You can only write to 3 registers when not in CONFIG_MODE
	//Page0::0x3D(op_mode), Page1::0x10, and Page1::0x0F
	if (running) {
		if (_page == 0 && _register != 0x3D) return false;
		if (_page == 1 && (_register != 0x10 || _register != 0x0F)) return false;
	}

	//Record any change to units of measure
	if (_register == 0x3B) meas_units = _data;

	bool ret;
	if (page0 != _page) {
		writePage(_page);
		page0 = !page0;
	}

	ret = write(_register, _data);
	//Min delay between write is 2 micros in normal, 450micros in low_power or suspend
	//Shouldn't be an issue since writes are for setup
	if (opmode == POWER_MODE::NORMAL) delayMicroseconds(2);
	else delayMicroseconds(450);
	if (page0 != _page) writePage(page0);

	if (ret) return true;
	return false;
}
**/
bool BNO055::writePage(uint8_t _page)
{
	if (_page > 1) return false;

	//bypass issue of register checking while running
	Wire.beginTransmission(SLADD);
	Wire.write(0x07);
	Wire.write(_page);
	bool ret = Wire.endTransmission();

	//Min delay between write is 2 micros in normal, 450micros in low_power or suspend
	//Shouldn't be an issue since writes are for setup
	if (opmode == uint8_t(POWER_MODE::NORMAL)) delayMicroseconds(2);
	else delayMicroseconds(450);

	if (ret && _page == 0) page0 = true;
	if (ret && _page == 1) page0 = false;
	return ret;
}

//uint8_t BNO055::readPage()
//{
//	if (page0) return 0;
//	else return 1;
//}

bool BNO055::orientation(PLACEMENT _placement)
{
	uint8_t config, sign;
	if (running) return false;

	switch (_placement) {
	case PLACEMENT::P0:
		config = 0x21;
		sign = 0x04;
		break;
	case PLACEMENT::P1:
		config = 0x24;
		sign = 0x00;
		break;
	case PLACEMENT::P2:
		config = 0x24;
		sign = 0x06;
		break;
	case PLACEMENT::P3:
		config = 0x21;
		sign = 0x02;
		break;
	case PLACEMENT::P4:
		config = 0x24;
		sign = 0x03;
		break;
	case PLACEMENT::P5:
		config = 0x21;
		sign = 0x01;
		break;
	case PLACEMENT::P6:
		config = 0x21;
		sign = 0x07;
		break;
	case PLACEMENT::P7:
		config = 0x24;
		sign = 0x05;
		break;
	default:
		config = 0x24;
		sign = 0x00;
		break;
	}

	write(Page0::AXIS_SIGN_CONFIG, config);
	write(Page0::AXIS_SIGN_REMAPPED, sign);

	if (read(Page0::AXIS_SIGN_CONFIG) == config) //two ifs, if config isn't right we just return false
		if (read(Page0::AXIS_SIGN_REMAPPED) == sign) return true; //otherwise we check axis signs
	return false;
}

bool BNO055::orientation(uint8_t _Axis_Config, uint8_t _Axis_Remap)
{
	if (running) return false;

	write(Page0::AXIS_SIGN_CONFIG, _Axis_Config);
	write(Page0::AXIS_SIGN_REMAPPED, _Axis_Remap);

	if (read(Page0::AXIS_SIGN_CONFIG) == _Axis_Config)
		if (read(Page0::AXIS_SIGN_REMAPPED) == _Axis_Remap) return true;
	return false;
}

bool BNO055::setAddress(bool _High)
{
	if (addresspin == 255) return false;

	digitalWrite(addresspin, _High);
	if (digitalRead(addresspin)) SLADD = 0x29;
	else SLADD = 0x28;

	return true;
}

bool BNO055::reset()
{
	if (resetpin == 255) return false;

	digitalWrite(resetpin, LOW);
	delayMicroseconds(1); //Needs to be low at least 20ns, should be fine with no delay but ehh......
	digitalWrite(resetpin, HIGH);
	delay(700); //reset to config time is 650ms, do 700 to be safe

	running = false;
	page0 = true;
	meas_units = 0x80; //default
	return true;
}

bool BNO055::setOperationMode(OPERATION_MODE _Operation_Mode)
{
	bool temp = running;
	if (_Operation_Mode == OPERATION_MODE::CONFIG) running = false;
	else running = true;

	uint8_t ret = write(Page0::OPERATION_MODE_REG, uint8_t(_Operation_Mode));

	if (temp && _Operation_Mode == OPERATION_MODE::CONFIG) delay(19); //switch from any mode to config requires 19ms
	if (!temp && _Operation_Mode != OPERATION_MODE::CONFIG) delay(7); //switch from config to any mode requires 7ms

	return ret;
}

bool BNO055::setPowerMode(POWER_MODE _Power_Mode)
{
	return write(Page0::POWER_MODE_REG, uint8_t(_Power_Mode));
}

bool BNO055::setUnits(uint8_t _UnitSel)
{
	if (running) return false;

	meas_units = _UnitSel;
	return write(Page0::UNIT_SELECTION, _UnitSel);
}

bool BNO055::setUnits(bool _Acceleration, bool _Angular_rate, bool _Euler_angles, bool _Temperature, bool _Fusion_Format)
{
	if (running) return false;

	uint8_t units = 0;
	units |= _Acceleration;
	units |= (_Angular_rate << 1);
	units |= (_Euler_angles << 2);
	units |= (_Temperature << 4);
	units |= (_Fusion_Format << 7);

	meas_units = units;

	return write(Page0::UNIT_SELECTION, units);
}

bool BNO055::setTempSource(bool Gyro)
{
	if (running) return false;

	return write(Page0::TEMP_SOURCE, Gyro);
}

bool BNO055::getTempSource()
{
	return read(Page0::TEMP_SOURCE);
}

int8_t BNO055::getTemp()
{
	if ((meas_units & 0x10)) return read(Page0::TEMP) * 2; //F*
	return read(Page0::TEMP); //C*
}

bool BNO055::getEuler(float *_data)
{
	uint8_t datastor[6]{ 0,0,0,0,0,0 };
	bool ret = read(Page0::EULER_HEADING_LSB, 6, datastor);

	if (meas_units & 0x04) {//Radians
		_data[0] = int16_t((datastor[1] << 8) | datastor[0]) / 900.0;
		_data[1] = int16_t((datastor[3] << 8) | datastor[2]) / 900.0;
		_data[2] = int16_t((datastor[5] << 8) | datastor[4]) / 900.0;
	}
	else //Degrees
	{
		_data[0] = int16_t((datastor[1] << 8) | datastor[0]) / 16.0;
		_data[1] = int16_t((datastor[3] << 8) | datastor[2]) / 16.0;
		_data[2] = int16_t((datastor[5] << 8) | datastor[4]) / 16.0;
	}
	return ret;
}

//Returns calculated pitch in degrees or radians depending on UNIT_SELECTION
float BNO055::getEulerPitch() {
	if (meas_units & 0x04) return (read16(Page0::EULER_PITCH_LSB)) / 900.0;
	return (read16(Page0::EULER_PITCH_LSB)) / 16.0;
}

//Returns calculated heading in degrees or radians depending on UNIT_SELECTION
float BNO055::getEulerHeading() {
	if (meas_units & 0x04) return (read16(Page0::EULER_HEADING_LSB)) / 900.0;
	return (read16(Page0::EULER_HEADING_LSB)) / 16.0;
}

//Returns calculated roll in degrees or radians depending on UNIT_SELECTION
float BNO055::getEulerRoll() {
	if (meas_units & 0x04) return (read16(Page0::EULER_ROLL_LSB)) / 900.0;
	return (read16(Page0::EULER_ROLL_LSB)) / 16.0;
}

bool BNO055::getGravity(float *_data)
{
	uint8_t datastor[6]{ 0,0,0,0,0,0 };
	bool ret = read(Page0::GRAV_XDATA_LSB, 6, datastor);

	if (meas_units & 0x01) { // mg
		_data[0] = int16_t((datastor[1] << 8) | datastor[0]);
		_data[1] = int16_t((datastor[3] << 8) | datastor[2]);
		_data[2] = int16_t((datastor[5] << 8) | datastor[4]);
	}
	else // m/s�
	{
		_data[0] = int16_t((datastor[1] << 8) | datastor[0]) / 100.0;
		_data[1] = int16_t((datastor[3] << 8) | datastor[2]) / 100.0;
		_data[2] = int16_t((datastor[5] << 8) | datastor[4]) / 100.0;
	}
	return ret;
}

//Gravity vector Z in mg or m/s� depending on UNIT_SELECTION
float BNO055::getGravityVectorZ()
{
	if (meas_units & 0x01) return (read16(Page0::GRAV_ZDATA_LSB)); // mg
	return (read16(Page0::GRAV_ZDATA_LSB)) / 100.0; // m/s�
}

//Gravity vector Y in mg or m/s� depending on UNIT_SELECTION
float BNO055::getGravityVectorY()
{
	if (meas_units & 0x01) return (read16(Page0::GRAV_YDATA_LSB)); // mg
	return (read16(Page0::GRAV_YDATA_LSB)) / 100.0; // m/s�
}

//Gravity vector X in mg or m/s� depending on UNIT_SELECTION
float BNO055::getGravityVectorX()
{
	if (meas_units & 0x01) return (read16(Page0::GRAV_XDATA_LSB)); // mg
	return (read16(Page0::GRAV_XDATA_LSB)) / 100.0; // m/s�
}

bool BNO055::getLinearAccel(float *_data)
{
	uint8_t datastor[6]{ 0,0,0,0,0,0 };
	bool ret = read(Page0::LINEAR_ACCEL_XDATA_LSB, 6, datastor);

	if (meas_units & 0x01) { // mg
		_data[0] = int16_t((datastor[1] << 8) | datastor[0]);
		_data[1] = int16_t((datastor[3] << 8) | datastor[2]);
		_data[2] = int16_t((datastor[5] << 8) | datastor[4]);
	}
	else // m/s�
	{
		_data[0] = int16_t((datastor[1] << 8) | datastor[0]) / 100.0;
		_data[1] = int16_t((datastor[3] << 8) | datastor[2]) / 100.0;
		_data[2] = int16_t((datastor[5] << 8) | datastor[4]) / 100.0;
	}
	return ret;
}

//Returns calculated Linear accel for Z axis, in mg or m/s�(determined by UNIT_SELECTION)
float BNO055::getLinearAccelZ()
{
	if (meas_units & 0x01) return (read16(Page0::LINEAR_ACCEL_ZDATA_LSB)); // mg
	return (read16(Page0::LINEAR_ACCEL_ZDATA_LSB)) / 100.0; // m/s�
}

//Returns calculated Linear accel for Y axis, in mg or m/s�(determined by UNIT_SELECTION)
float BNO055::getLinearAccelY()
{
	if (meas_units & 0x01) return (read16(Page0::LINEAR_ACCEL_YDATA_LSB)); // mg
	return (read16(Page0::LINEAR_ACCEL_YDATA_LSB)) / 100.0; // m/�

}

//Returns calculated Linear accel for X axis, in mg or m/s�(determined by UNIT_SELECTION)
float BNO055::getLinearAccelX()
{
	if (meas_units & 0x01) return (read16(Page0::LINEAR_ACCEL_XDATA_LSB)); // mg
	return (read16(Page0::LINEAR_ACCEL_XDATA_LSB)) / 100.0; // m/s�

}

bool BNO055::getQuaternion(double *_data)
{
	uint8_t datastor[8]{ 0,0,0,0,0,0,0,0 };
	bool ret = read(Page0::QUAT_WDATA_LSB, 8, datastor);

	_data[0] = int16_t((datastor[1] << 8) | datastor[0]) / 16384.0;
	_data[1] = int16_t((datastor[3] << 8) | datastor[2]) / 16384.0;
	_data[2] = int16_t((datastor[5] << 8) | datastor[4]) / 16384.0;
	_data[3] = int16_t((datastor[7] << 8) | datastor[6]) / 16384.0;

	return ret;
}

double BNO055::getQuatZ()
{
	return (read16(Page0::QUAT_ZDATA_LSB)) / 16384.0;
}

//returns quat Y value
double BNO055::getQuatY()
{
	return (read16(Page0::QUAT_YDATA_LSB)) / 16384.0;
}

//returns quat X value
double BNO055::getQuatX()
{
	return (read16(Page0::QUAT_XDATA_LSB)) / 16384.0;
}

//returns quat W value
double BNO055::getQuatW()
{
	return (read16(Page0::QUAT_WDATA_LSB)) / 16384.0;
}

bool BNO055::getGyro(float *_data)
{
	uint8_t datastor[6]{ 0,0,0,0,0,0 };
	bool ret = read(Page0::GRAV_XDATA_LSB, 6, datastor);

	if (meas_units & 0x02) { // RPS
		_data[0] = int16_t((datastor[1] << 8) | datastor[0]) / 900.0;
		_data[1] = int16_t((datastor[3] << 8) | datastor[2]) / 900.0;
		_data[2] = int16_t((datastor[5] << 8) | datastor[4]) / 900.0;
	}
	else // DPS
	{
		_data[0] = int16_t((datastor[1] << 8) | datastor[0]) / 16.0;
		_data[1] = int16_t((datastor[3] << 8) | datastor[2]) / 16.0;
		_data[2] = int16_t((datastor[5] << 8) | datastor[4]) / 16.0;
	}
	return ret;
}

//returns Gyro Z data in DPS or RPS, dependent on UNIT_SELECTION
float BNO055::getGyroZ()
{
	if (meas_units & 0x02) return (read16(Page0::GYRO_ZDATA_LSB)) / 900;//RPS
	return (read16(Page0::GYRO_ZDATA_LSB)) / 16;//dps
}

//returns Gyro Y data in DPS or RPS, dependent on UNIT_SELECTION
float BNO055::getGyroY()
{
	if (meas_units & 0x02) return (read16(Page0::GYRO_YDATA_LSB)) / 900;//RPS
	return (read16(Page0::GYRO_YDATA_LSB)) / 16;//dps
}

//returns Gyro X data in DPS or RPS, dependent on UNIT_SELECTION
float BNO055::getGyroX()
{
	if (meas_units & 0x02) return (read16(Page0::GYRO_XDATA_LSB)) / 900;//RPS
	return (read16(Page0::GYRO_XDATA_LSB)) / 16;//dps
}

bool BNO055::getAccel(float *_data)
{
	uint8_t datastor[6]{ 0,0,0,0,0,0 };
	bool ret = read(Page0::ACCEL_XDATA_LSB, 6, datastor);

	if (meas_units & 0x01) { // mg
		_data[0] = int16_t((datastor[1] << 8) | datastor[0]);
		_data[1] = int16_t((datastor[3] << 8) | datastor[2]);
		_data[2] = int16_t((datastor[5] << 8) | datastor[4]);
	}
	else // m/s�
	{
		_data[0] = int16_t((datastor[1] << 8) | datastor[0]) / 100.0;
		_data[1] = int16_t((datastor[3] << 8) | datastor[2]) / 100.0;
		_data[2] = int16_t((datastor[5] << 8) | datastor[4]) / 100.0;
	}
	return ret;
}

//returns accel Z data based on UNIT_SELECTION, mg or m/s�
float BNO055::getAccelZ()
{
	if (meas_units & 0x01)  return (read16(Page0::ACCEL_ZDATA_LSB));//mg
	return (read16(Page0::ACCEL_ZDATA_LSB)) / 100.0;// m/s�
}

//returns accel X data based on UNIT_SELECTION, mg or m/s�
float BNO055::getAccelY()
{
	if (meas_units & 0x01)  return (read16(Page0::ACCEL_YDATA_LSB));//mg
	return (read16(Page0::ACCEL_YDATA_LSB)) / 100.0;// m/s�
}

//returns accel X data based on UNIT_SELECTION, mg or m/s�
float BNO055::getAccelX()
{
	if (meas_units & 0x01)  return (read16(Page0::ACCEL_XDATA_LSB));//mg
	return (read16(Page0::ACCEL_XDATA_LSB)) / 100.0;// m/s�
}

bool BNO055::getMag(float *_data)
{
	uint8_t datastor[6]{ 0,0,0,0,0,0 };
	bool ret = read(Page0::MAG_XDATA_LSB, 6, datastor);

	_data[0] = int16_t((datastor[1] << 8) | datastor[0]) / 16.0;
	_data[1] = int16_t((datastor[3] << 8) | datastor[2]) / 16.0;
	_data[2] = int16_t((datastor[5] << 8) | datastor[4]) / 16.0;

	return ret;
}

//returns mag Z value in uT
float BNO055::getMagZ()
{
	return (read16(Page0::MAG_ZDATA_LSB)) / 16.0;
}
//returns mag Y value in uT
float BNO055::getMagY()
{
	return (read16(Page0::MAG_YDATA_LSB)) / 16.0;
}
//returns mag X value in uT
float BNO055::getMagX()
{
	return (read16(Page0::MAG_XDATA_LSB)) / 16.0;
}

bool BNO055::saveCalib()
{
#ifdef MyCalibrations
	return printCalib();
#else
#ifdef EEPROM_h
	return saveEEPROM();
#else
	return printCalib();
#endif
#endif
}

bool BNO055::loadCalib()
{
#ifdef MyCalibrations
	return loadMyCal();
#else
#ifdef EEPROM_h
	return loadEEPROM();
#else
	return false;
#endif
#endif
}

#ifdef MyCalibrations
bool BNO055::loadMyCal()
{
	uint8_t OPMODE;

	OPMODE = read(Page0::OPERATION_MODE_REG);
	if (OPMODE != 0x00) setOperationMode(OPERATION_MODE::CONFIG);
	if (read(Page0::OPERATION_MODE_REG) != uint8_t(OPERATION_MODE::CONFIG)) return false; //couldn't set config mode
	delay(20); //delay to allow to switch to config

	for (size_t i = 0; i < 22; ++i)
	{
		write(Page0(i + 0x55), MyCal[i]);
	}

	write(Page0::OPERATION_MODE_REG, OPMODE);
	delay(8); //wait to come out of config
	return true;
}
#else
#ifdef EEPROM_h
bool BNO055::saveEEPROM()
{
	uint8_t temp = 0;
	for (size_t i = 0; i<22; ++i)
	{
		temp = read(Page0(i + 0x55));
		EEPROM.write(i + eepromaddress, temp);
	}
	return true;
}

bool BNO055::loadEEPROM()
{
	uint8_t OPMODE = 0;
	uint8_t temp = 0;

	OPMODE = read(Page0::OPERATION_MODE_REG);
	if (OPMODE != 0x00) write(Page0::OPERATION_MODE_REG, OPERATION_MODE::CONFIG);
	delay(20); //delay to allow time to switch to config mode

	if (read(Page0::OPERATION_MODE_REG) != OPERATION_MODE::CONFIG) return false; //couldn't set config mode

	for (size_t i = 0; i<22; ++i)
	{
		EEPROM.read(i + eepromaddress, temp);
		write(Page0(i + 0x55), temp);
	}

	write(Page0::OPERATION_MODE_REG, OPMODE);
	delay(8); //wait to come out of config
	return true;
}
#endif
#endif

#ifndef EEPROM_h
bool BNO055::printCalib()
{
	uint8_t temp;
	if (!Serial) return false;

	Serial.println("Calibration Profile, copy/paste lines to BNODEFS.h:");
	Serial.println("#define MyCalibrations");
	Serial.print("static const uint8_t MyCal[] = { ");

	for (size_t i = 0; i<22; ++i)
	{
		temp = read(Page0(i + 0x55));
		Serial.print(temp, DEC);
		if (i != 21) Serial.print(", ");
		else Serial.println(" };");
	}
	return true;
}
#endif

/////////////////////////////////////////////////////////////
//These are Page1 register functions.
//
//
//These are mainly for interrupt settings and manual sensor config
//for non-fusion operating modes
//These all reselect whichever page was selected, doesn't write page if already page 1
/////////////////////////////////////////////////////////////
bool BNO055::setInterruptEnable(uint8_t _IntEnable)
{
	return write(Page1::INTERRUPT_ENABLE, _IntEnable); //write data
}

uint8_t BNO055::getInterruptEnable()
{
	return read(Page1::INTERRUPT_ENABLE); //write data
}

bool BNO055::setInterruptMask(uint8_t _IntMask)
{
	return write(Page1::INTERRUPT_MASK, _IntMask); //write data
}

uint8_t BNO055::getInterruptMask()
{
	return read(Page1::INTERRUPT_MASK);
}

bool BNO055::setMagConfig(uint8_t _Mag_Config)
{
	if (running) return false; //can't set if not in configmode

	return write(Page1::MAG_CONFIG, _Mag_Config);
}

uint8_t BNO055::getMagConfig()
{
	return read(Page1::MAG_CONFIG);

}

bool BNO055::setGyroConfig(uint8_t _Config0, uint8_t _Config1)
{
	if (running) return false;
	uint8_t ret = 0;

	ret = write(Page1::GYRO_CONFIG_0, _Config0);
	ret |= write(Page1::GYRO_CONFIG_1, _Config1);

	return ret;
}

uint8_t BNO055::getGyroConfig0()
{
	return read(Page1::GYRO_CONFIG_0);
}

uint8_t BNO055::getGyroConfig1()
{
	return read(Page1::GYRO_CONFIG_1);
}

bool BNO055::setGyroSleepConfig(uint8_t _SleepData)
{
	if (running) return false;

	return write(Page1::GYRO_SLEEP_CONFIG, _SleepData);
}

uint8_t BNO055::getGyroSleepConfig()
{
	return read(Page1::GYRO_SLEEP_CONFIG);
}

bool BNO055::setGyroInterruptSettings(uint8_t _GyroIntSettings)
{
	if (running) return false;

	return write(Page1::GYRO_INTERRUPT_SETUP, _GyroIntSettings);
}

uint8_t BNO055::getGyroInterruptSettings()
{
	return read(Page1::GYRO_INTERRUPT_SETUP);
}

bool BNO055::setGyroHR_XThreshold(uint8_t _HighRateX)
{
	if (running) return false;

	return write(Page1::GYRO_XHIGHRATE_THRESHOLD, _HighRateX);
}

uint8_t BNO055::getGyroHR_XThreshold()
{
	return read(Page1::GYRO_XHIGHRATE_THRESHOLD);
}

bool BNO055::setGyroHR_XDuration(uint8_t _HighRateXDur)
{
	if (running) return false;

	return write(Page1::GYRO_XHIGHRATE_DURATION, _HighRateXDur);
}

uint8_t BNO055::getGyroHR_XDuration()
{
	return read(Page1::GYRO_XHIGHRATE_DURATION);
}

bool BNO055::setGyroHR_YThreshold(uint8_t _HighRateY)
{
	if (running) return false;

	return write(Page1::GYRO_YHIGHRATE_THRESHOLD, _HighRateY);
}

uint8_t BNO055::getGyroHR_YThreshold()
{
	return read(Page1::GYRO_YHIGHRATE_THRESHOLD);
}

bool BNO055::setGyroHR_YDuration(uint8_t _HighRateYDur)
{
	if (running) return false;

	return write(Page1::GYRO_YHIGHRATE_DURATION, _HighRateYDur);
}

uint8_t BNO055::getGyroHR_YDuration()
{
	return read(Page1::GYRO_YHIGHRATE_DURATION);
}

bool BNO055::setGyroHR_ZThreshold(uint8_t _HighRateZ)
{
	if (running) return false;

	return write(Page1::GYRO_ZHIGHRATE_THRESHOLD, _HighRateZ);
}

uint8_t BNO055::getGyroHR_ZThreshold()
{
	return read(Page1::GYRO_ZHIGHRATE_THRESHOLD);
}

bool BNO055::setGyroHR_ZDuration(uint8_t _HighRateZDur)
{
	if (running) return false;

	return write(Page1::GYRO_ZHIGHRATE_DURATION, _HighRateZDur);
}

uint8_t BNO055::getGyroHR_ZDuration()
{
	return read(Page1::GYRO_ZHIGHRATE_DURATION);
}

bool BNO055::setGyroAnyMotionThreshold(uint8_t _AMThresh)
{
	if (running) return false;

	return write(Page1::GYRO_ANYMOTION_THRESHOLD, _AMThresh);
}

uint8_t BNO055::getGyroAnyMotionThreshold()
{
	return read(Page1::GYRO_ANYMOTION_THRESHOLD);
}

bool BNO055::setGyroAnyMotionSettings(uint8_t _AMSettings)
{
	if (running) return false;

	return write(Page1::GYRO_ANYMOTION_SETUP, _AMSettings);
}

uint8_t BNO055::getGyroAnyMotionSettings()
{
	return read(Page1::GYRO_ANYMOTION_SETUP);
}

bool BNO055::setAccConfig(uint8_t _ACC_Config)
{
	if (running) return false;

	return write(Page1::ACCEL_CONFIG, _ACC_Config);
}

bool BNO055::setAccConfig(ACC_RANGE _G_Range, ACC_BANDWIDTH _Bandwidth, ACC_OPMODE _Op_Mode)
{
	if (running) return false;

	return write(Page1::ACCEL_CONFIG, uint8_t(_G_Range) | uint8_t(_Bandwidth) | uint8_t(_Op_Mode));
}

uint8_t BNO055::getAccConfig()
{
	return read(Page1::ACCEL_CONFIG);
}

bool BNO055::setAccSleepConfig(uint8_t _SleepData)
{
	if (running) return false;

	return write(Page1::ACCEL_SLEEP_CONFIG, _SleepData);
}

uint8_t BNO055::getAccSleepConfig()
{
	return read(Page1::ACCEL_SLEEP_CONFIG);
}

bool BNO055::setAccAnyMotionThreshold(uint8_t _AMTheshold)
{
	if (running) return false;

	return write(Page1::ACCEL_ANYMOTION_THRESHOLD, _AMTheshold);
}

uint8_t BNO055::getAccAnyMotionThreshold()
{
	return read(Page1::ACCEL_ANYMOTION_THRESHOLD);
}

bool BNO055::setAccInterruptSettings(uint8_t _IntSettings)
{
	if (running) return false;

	return write(Page1::ACCEL_INTERRUPT_SETTINGS, _IntSettings);
}

uint8_t BNO055::getAccInterruptSettings()
{
	return read(Page1::ACCEL_INTERRUPT_SETTINGS);
}

bool BNO055::setAccHighGDuration(uint8_t _HighGDuration)
{
	if (running) return false;

	return write(Page1::ACCEL_HIGHG_DURATION, _HighGDuration);
}

uint8_t BNO055::getAccHighGDuration()
{
	return read(Page1::ACCEL_HIGHG_DURATION);
}

bool BNO055::setAccHighGThreshold(uint8_t _HighGThreshold)
{
	if (running) return false;

	return write(Page1::ACCEL_HIGHG_THRESHOLD, _HighGThreshold);
}

uint8_t BNO055::getAccHighGThreshold()
{
	return read(Page1::ACCEL_HIGHG_THRESHOLD);
}

bool BNO055::setAccNoMotionThreshold(uint8_t _NoMotionThreshold)
{
	if (running) return false;

	return write(Page1::ACCEL_NOMOTION_THRESHOLD, _NoMotionThreshold);
}

uint8_t BNO055::getAccNoMotionThreshold()
{
	return read(Page1::ACCEL_NOMOTION_THRESHOLD);
}

bool BNO055::setAccNoMotionSetting(uint8_t _SloNoMotionSet)
{
	if (running) return false;

	return write(Page1::ACCEL_NOMOTION_SETUP, _SloNoMotionSet);
}

uint8_t BNO055::getAccNoMotionSetting()
{
	return read(Page1::ACCEL_NOMOTION_SETUP);
}
