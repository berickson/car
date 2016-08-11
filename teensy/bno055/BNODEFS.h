#pragma once
#include <inttypes.h>

#define MyCalibrations
static const uint8_t MyCal[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 224, 1 };

//These are Page 0 registers
////////////////////////////
//Mag, and Accel radius registers
enum Page0 : const uint8_t {
	MAG_RADIUS_MSB = 0x6A,
	MAG_RADIUS_LSB = 0x69,
	ACCEL_RADIUS_MSB = 0x68,
	ACCEL_RADIUS_LSB = 0x67,

	//Gyro offset registers
	GYRO_ZOFFSET_MSB = 0x66,
	GYRO_ZOFFSET_LSB = 0x65,
	GYRO_YOFFSET_MSB = 0x64,
	GYRO_YOFFSET_LSB = 0x63,
	GYRO_XOFFSET_MSB = 0x62,
	GYRO_XOFFSET_LSB = 0x61,

	//Mag offset registers
	MAG_ZOFFSET_MSB = 0x60,
	MAG_ZOFFSET_LSB = 0x5F,
	MAG_YOFFSET_MSB = 0x5E,
	MAG_YOFFSET_LSB = 0x5D,
	MAG_XOFFSET_MSB = 0x5C,
	MAG_XOFFSET_LSB = 0x5B,

	//Accel offset registers
	ACCEL_ZOFFSET_MSB = 0x5A,
	ACCEL_ZOFFSET_LSB = 0x59,
	ACCEL_YOFFSET_MSB = 0x58,
	ACCEL_YOFFSET_LSB = 0x57,
	ACCEL_XOFFSET_MSB = 0x56,
	ACCEL_XOFFSET_LSB = 0x55,

	//Control registers(This section is fairly large...)
	AXIS_SIGN_REMAPPED = 0x42,
	AXIS_SIGN_CONFIG = 0x41,
	TEMP_SOURCE = 0x40, //0x02 default
	SYSTEM_TRIGGER = 0x3F,
	POWER_MODE_REG = 0x3E,
	OPERATION_MODE_REG = 0x3D, //0x1C default
	UNIT_SELECTION = 0x3B, //0x80 default
	SYSTEM_ERROR = 0x3A,
	SYSTEM_STATUS = 0x39,
	SYSTEM_CLOCK_STATUS = 0x38,
	INTERRUPT_STATUS = 0x37,
	SELFTEST_RESULT = 0x36, //0x0F default
	CALIBRATION_STATUS = 0x35,
	TEMP = 0x34,

	//Grav vector data
	GRAV_ZDATA_MSB = 0x33,
	GRAV_ZDATA_LSB = 0x32,
	GRAV_YDATA_MSB = 0x31,
	GRAV_YDATA_LSB = 0x30,
	GRAV_XDATA_MSB = 0x2F,
	GRAV_XDATA_LSB = 0x2E,

	//Linear Accel data
	LINEAR_ACCEL_ZDATA_MSB = 0x2D,
	LINEAR_ACCEL_ZDATA_LSB = 0x2C,
	LINEAR_ACCEL_YDATA_MSB = 0x2B,
	LINEAR_ACCEL_YDATA_LSB = 0x2A,
	LINEAR_ACCEL_XDATA_MSB = 0x29,
	LINEAR_ACCEL_XDATA_LSB = 0x28,

	//Quaternion data
	QUAT_ZDATA_MSB = 0x27,
	QUAT_ZDATA_LSB = 0x26,
	QUAT_YDATA_MSB = 0x25,
	QUAT_YDATA_LSB = 0x24,
	QUAT_XDATA_MSB = 0x23,
	QUAT_XDATA_LSB = 0x22,
	QUAT_WDATA_MSB = 0x21,
	QUAT_WDATA_LSB = 0x20,

	//Pitch, roll, heading data
	EULER_PITCH_MSB = 0x1F,
	EULER_PITCH_LSB = 0x1E,
	EULER_ROLL_MSB = 0x1D,
	EULER_ROLL_LSB = 0x1C,
	EULER_HEADING_MSB = 0x1B,
	EULER_HEADING_LSB = 0x1A,

	//Gyro data
	GYRO_ZDATA_MSB = 0x19,
	GYRO_ZDATA_LSB = 0x18,
	GYRO_YDATA_MSB = 0x17,
	GYRO_YDATA_LSB = 0x16,
	GYRO_XDATA_MSB = 0x15,
	GYRO_XDATA_LSB = 0x14,

	//Mag data
	MAG_ZDATA_MSB = 0x13,
	MAG_ZDATA_LSB = 0x12,
	MAG_YDATA_MSB = 0x11,
	MAG_YDATA_LSB = 0x10,
	MAG_XDATA_MSB = 0x0F,
	MAG_XDATA_LSB = 0x0E,

	//Accel data
	ACCEL_ZDATA_MSB = 0x0D,
	ACCEL_ZDATA_LSB = 0x0C,
	ACCEL_YDATA_MSB = 0x0B,
	ACCEL_YDATA_LSB = 0x0A,
	ACCEL_XDATA_MSB = 0x09,
	ACCEL_XDATA_LSB = 0x08,

	//ID registers
	BOOTLOADER_ID = 0x06,
	SW_REVISION_MSB = 0x05, //0x03 SW major revision
	SW_REVISION_LSB = 0x04, //0x08 SW minor revision
	GYRO_ID = 0x03, //0x0F default
	MAG_ID = 0x02, //0x32 default
	ACCEL_ID = 0x01, //0xFB default
	CHIP_ID = 0x00 //0xA0 default
};

//These are page 1 registers
////////////////////////////
enum class Page1 : const uint8_t {
	//BNO055 Unique ID range
	UNIQUE_ID_START = 0x50,
	UNIQUE_ID_END = 0x5F,

	//Gyro interrupt settings
	GYRO_ANYMOTION_SETUP = 0x1F,
	GYRO_ANYMOTION_THRESHOLD = 0x1E,
	GYRO_ZHIGHRATE_DURATION = 0x1D,
	GYRO_ZHIGHRATE_THRESHOLD = 0x1C,
	GYRO_YHIGHRATE_DURATION = 0x1B,
	GYRO_YHIGHRATE_THRESHOLD = 0x1A,
	GYRO_XHIGHRATE_DURATION = 0x19,
	GYRO_XHIGHRATE_THRESHOLD = 0x18,
	GYRO_INTERRUPT_SETUP = 0x17,

	//Accel interrupt settings
	ACCEL_NOMOTION_SETUP = 0x16,
	ACCEL_NOMOTION_THRESHOLD = 0x15,
	ACCEL_HIGHG_THRESHOLD = 0x14,
	ACCEL_HIGHG_DURATION = 0x13,
	ACCEL_INTERRUPT_SETTINGS = 0x12,
	ACCEL_ANYMOTION_THRESHOLD = 0x11,

	//CONFIG REGISTERS
	INTERRUPT_ENABLE = 0x10,
	INTERRUPT_MASK = 0x0F,
	GYRO_SLEEP_CONFIG = 0x0D,
	ACCEL_SLEEP_CONFIG = 0x0C,
	GYRO_CONFIG_1 = 0x0B,
	GYRO_CONFIG_0 = 0x0A,
	MAG_CONFIG = 0x09,
	ACCEL_CONFIG = 0x08
};

enum class POWER_MODE : const uint8_t {
	NORMAL = 0x00,
	LOW_POWER,
	SUSPEND
};
enum class OPERATION_MODE : const uint8_t {
	CONFIG = 0x00,
	ACC_ONLY,
	MAG_ONLY,
	GYRO_ONLY,
	ACC_MAG,
	ACC_GYRO,
	MAG_GYRO,
	ACC_MAG_GYRO,
	FUSION_IMU,
	FUSION_COMPASS,
	FUSION_M4G,
	FUSION_NDOF_SLOWCAL,
	FUSION_NDOF_NORMAL
};

//axis can be redefined as any other axis
enum class AXIS : const uint8_t {X = 0x00 , Y, Z};
//axis orientation can be redefined as positive or negative for all axis
enum class AXIS_SIGN : const uint8_t {Positive = 0x00, Negative};
//See datasheet for what orientations these describe
enum class PLACEMENT: const uint8_t{ P0, P1, P2, P3, P4, P5, P6, P7 };

//Accel Config
enum class ACC_RANGE : const uint8_t {G2 = 0x00, G4 = 0x01, G8 = 0x02, G16 = 0x03};
enum class ACC_BANDWIDTH : const uint8_t {F7P81 = 0x00, F15P63 = 0x04, F31P25 = 0x08, F62P5 = 0x0C, HZ125 = 0x10, HZ250 = 0x14, HZ500 = 0x18, HZ1000 = 0x1C};
enum class ACC_OPMODE : const uint8_t {Normal = 0x00, Suspend = 0x20, LowPower1 = 0x40, Standby = 0x60, LowPower2 = 0x80, DeepSuspend = 0xA0};

//Gyro Config0
enum class GYRO_RANGE : const uint8_t {DPS125 = 0x04, DPS250 = 0x03, DPS500 = 0x02, DPS1000 = 0x01, DPS2000 = 0x00};
enum class GYRO_BANDWIDTH : const uint8_t {HZ12 = 0x28, HZ23 = 0x20, HZ32 = 0x38, HZ47 = 0x18, HZ64 = 0x30, HZ116 = 0x10, HZ230 = 0x08, HZ523 = 0x00};
//Gyro Config1
enum class GYRO_OPMODE : const uint8_t {Normal = 0x00, FastPowerUp = 0x01, DeepSuspend = 0x02, Suspend = 0x03, AdvancedPowersave = 0x04};

//Mag Config
enum class MAG_OUTPUTRATE : const uint8_t {HZ2=0x00, HZ6, HZ8, HZ10, HZ15, HZ20, HZ25, HZ30};
enum class MAG_OPMODE : const uint8_t {Regular = 0x08, LowPower = 0x00, EnhancedRegular = 0x10, HighAccuracy = 0x18};
enum class MAG_POWERMODE : const uint8_t {Norma_ = 0x00, Sleep = 0x20, Suspend = 0x40, Force = 0x60};

/*
//These are Page 0 registers
////////////////////////////

//Mag, and Accel radius registers
#define MAG_RADIUS_MSB 0x6A
#define MAG_RADIUS_LSB 0x69
#define ACCEL_RADIUS_MSB 0x68
#define ACCEL_RADIUS_LSB 0x67

//Gyro offset registers
#define GYRO_ZOFFSET_MSB 0x66
#define GYRO_ZOFFSET_LSB 0x65
#define GYRO_YOFFSET_MSB 0x64
#define GYRO_YOFFSET_LSB 0x63
#define GYRO_XOFFSET_MSB 0x62
#define GYRO_XOFFSET_LSB 0x61

//Mag offset registers
#define MAG_ZOFFSET_MSB 0x60
#define MAG_ZOFFSET_LSB 0x5F
#define MAG_YOFFSET_MSB 0x5E
#define MAG_YOFFSET_LSB 0x5D
#define MAG_XOFFSET_MSB 0x5C
#define MAG_XOFFSET_LSB 0x5B

//Accel offset registers
#define ACCEL_ZOFFSET_MSB 0x5A
#define ACCEL_ZOFFSET_LSB 0x59
#define ACCEL_YOFFSET_MSB 0x58
#define ACCEL_YOFFSET_LSB 0x57
#define ACCEL_XOFFSET_MSB 0x56
#define ACCEL_XOFFSET_LSB 0x55

//Control registers(This section is fairly large...)
#define AXIS_SIGN_REMAPPED 0x42
#define AXIS_SIGN_CONFIG 0x41
#define TEMP_SOURCE 0x40 //0x02 default
#define SYSTEM_TRIGGER 0x3F
#define POWER_MODE 0x3E
#define OPERATION_MODE 0x3D //0x1C default
#define UNIT_SELECTION 0x3B //0x80 default
#define SYSTEM_ERROR 0x3A
#define SYSTEM_STATUS 0x39
#define SYSTEM_CLOCK_STATUS 0x38
#define INTERRUPT_STATUS 0x37
#define SELFTEST_RESULT 0x36 //0x0F default
#define CALIBRATION_STATUS 0x35
#define TEMP 0x34

//Grav vector data
#define GRAV_ZDATA_MSB 0x33
#define GRAV_ZDATA_LSB 0x32
#define GRAV_YDATA_MSB 0x31
#define GRAV_YDATA_LSB 0x30
#define GRAV_XDATA_MSB 0x2F
#define GRAV_XDATA_LSB 0x2E

//Linear Accel data
#define LINEAR_ACCEL_ZDATA_MSB 0x2D
#define LINEAR_ACCEL_ZDATA_LSB 0x2C
#define LINEAR_ACCEL_YDATA_MSB 0x2B
#define LINEAR_ACCEL_YDATA_LSB 0x2A
#define LINEAR_ACCEL_XDATA_MSB 0x29
#define LINEAR_ACCEL_XDATA_LSB 0x28

//Quaternion data
#define QUAT_ZDATA_MSB 0x27
#define QUAT_ZDATA_LSB 0x26
#define QUAT_YDATA_MSB 0x25
#define QUAT_YDATA_LSB 0x24
#define QUAT_XDATA_MSB 0x23
#define QUAT_XDATA_LSB 0x22
#define QUAT_WDATA_MSB 0x21
#define QUAT_WDATA_LSB 0x20

//Pitch, roll, heading data
#define EULER_PITCH_MSB 0x1F
#define EULER_PITCH_LSB 0x1E
#define EULER_ROLL_MSB 0x1D
#define EULER_ROLL_LSB 0x1C
#define EULER_HEADING_MSB 0x1B
#define EULER_HEADING_LSB 0x1A

//Gyro data
#define GYRO_ZDATA_MSB 0x19
#define GYRO_ZDATA_LSB 0x18
#define GYRO_YDATA_MSB 0x17
#define GYRO_YDATA_LSB 0x16
#define GYRO_XDATA_MSB 0x15
#define GYRO_XDATA_LSB 0x14

//Mag data
#define MAG_ZDATA_MSB 0x13
#define MAG_ZDATA_LSB 0x12
#define MAG_YDATA_MSB 0x11
#define MAG_YDATA_LSB 0x10
#define MAG_XDATA_MSB 0x0F
#define MAG_XDATA_LSB 0x0E

//Accel data
#define ACCEL_ZDATA_MSB 0x0D
#define ACCEL_ZDATA_LSB 0x0C
#define ACCEL_YDATA_MSB 0x0B
#define ACCEL_YDATA_LSB 0x0A
#define ACCEL_XDATA_MSB 0x09
#define ACCEL_XDATA_LSB 0x08

//ID registers, pa
#define PAGE_ID 0x07
#define BOOTLOADER_ID 0x06
#define SW_REVISION_MSB 0x05 //0x03 SW major revision
#define SW_REVISION_LSB 0x04 //0x08 SW minor revision
#define GYRO_ID 0x03 //0x0F default
#define MAG_ID 0x02 //0x32 default
#define ACCEL_ID 0x01 //0xFB default
#define CHIP_ID 0x00 //0xA0 default

//These are page 1 registers
////////////////////////////

//BNO055 Unique ID range
#define UNIQUE_ID_START 0x50
#define UNIQUE_ID_END 0x5F

//Gyro interrupt settings
#define GYRO_ANYMOTION_SETUP 0x1F
#define GYRO_ANYMOTION_THRESHOLD 0x1E
#define GYRO_ZHIGHRATE_DURATION 0x1D
#define GYRO_ZHIGHRATE_THRESHOLD 0x1C
#define GYRO_YHIGHRATE_DURATION 0x1B
#define GYRO_YHIGHRATE_THRESHOLD 0x1A
#define GYRO_XHIGHRATE_DURATION 0x19
#define GYRO_XHIGHRATE_THRESHOLD 0x18
#define GYRO_INTERRUPT_SETUP 0x17

//Accel interrupt settings
#define ACCEL_NOMOTION_SETUP 0x16
#define ACCEL_NOMOTION_THRESHOLD 0x15
#define ACCEL_HIGHG_THRESHOLD 0x14
#define ACCEL_HIGHG_DURATION 0x13
#define ACCEL_INTERRUPT_SETTINGS 0x12
#define ACCEL_ANYMOTION_THRESHOLD 0x11

//CONFIG REGISTERS
#define INTERRUPT_ENABLE 0x10
#define INTERRUPT_MASK 0x0F
#define GYRO_SLEEP_CONFIG 0x0D
#define ACCEL_SLEEP_CONFIG 0x0C
#define GYRO_CONFIG_1 0x0B
#define GYRO_CONFIG_0 0x0A
#define MAG_CONFIG 0x09
#define ACCEL_CONFIG 0x08

//Page ID, write to switch
#define PAGE_ID 0x07


//These are Page 0 registers
////////////////////////////

struct Page0 {
const uint8_t MAG_RADIUS_MSB = 0x6A;
const uint8_t MAG_RADIUS_LSB = 0x69;
const uint8_t ACCEL_RADIUS_MSB = 0x68;
const uint8_t ACCEL_RADIUS_LSB = 0x67;

//Gyro offset registers
const uint8_t GYRO_ZOFFSET_MSB = 0x66;
const uint8_t GYRO_ZOFFSET_LSB = 0x65;
const uint8_t GYRO_YOFFSET_MSB = 0x64;
const uint8_t GYRO_YOFFSET_LSB = 0x63;
const uint8_t GYRO_XOFFSET_MSB = 0x62;
const uint8_t GYRO_XOFFSET_LSB = 0x61;

//Mag offset registers
const uint8_t MAG_ZOFFSET_MSB = 0x60;
const uint8_t MAG_ZOFFSET_LSB = 0x5F;
const uint8_t MAG_YOFFSET_MSB = 0x5E;
const uint8_t MAG_YOFFSET_LSB = 0x5D;
const uint8_t MAG_XOFFSET_MSB = 0x5C;
const uint8_t MAG_XOFFSET_LSB = 0x5B;

//Accel offset registers
const uint8_t ACCEL_ZOFFSET_MSB = 0x5A;
const uint8_t ACCEL_ZOFFSET_LSB = 0x59;
const uint8_t ACCEL_YOFFSET_MSB = 0x58;
const uint8_t ACCEL_YOFFSET_LSB = 0x57;
const uint8_t ACCEL_XOFFSET_MSB = 0x56;
const uint8_t ACCEL_XOFFSET_LSB = 0x55;

//Control registers(This section is fairly large...)
const uint8_t AXIS_SIGN_REMAPPED = 0x42;
const uint8_t AXIS_SIGN_CONFIG = 0x41;
const uint8_t TEMP_SOURCE = 0x40; //0x02 default
const uint8_t SYSTEM_TRIGGER = 0x3F;
const uint8_t POWER_MODE = 0x3E;
const uint8_t OPERATION_MODE = 0x3D; //0x1C default
const uint8_t UNIT_SELECTION = 0x3B; //0x80 default
const uint8_t SYSTEM_ERROR = 0x3A;
const uint8_t SYSTEM_STATUS = 0x39;
const uint8_t SYSTEM_CLOCK_STATUS = 0x38;
const uint8_t INTERRUPT_STATUS = 0x37;
const uint8_t SELFTEST_RESULT = 0x36; //0x0F default
const uint8_t CALIBRATION_STATUS = 0x35;
const uint8_t TEMP = 0x34;

//Grav vector data
const uint8_t GRAV_ZDATA_MSB = 0x33;
const uint8_t GRAV_ZDATA_LSB = 0x32;
const uint8_t GRAV_YDATA_MSB = 0x31;
const uint8_t GRAV_YDATA_LSB = 0x30;
const uint8_t GRAV_XDATA_MSB = 0x2F;
const uint8_t GRAV_XDATA_LSB = 0x2E;

//Linear Accel data
const uint8_t LINEAR_ACCEL_ZDATA_MSB = 0x2D;
const uint8_t LINEAR_ACCEL_ZDATA_LSB = 0x2C;
const uint8_t LINEAR_ACCEL_YDATA_MSB = 0x2B;
const uint8_t LINEAR_ACCEL_YDATA_LSB = 0x2A;
const uint8_t LINEAR_ACCEL_XDATA_MSB = 0x29;
const uint8_t LINEAR_ACCEL_XDATA_LSB = 0x28;

//Quaternion data
const uint8_t QUAT_ZDATA_MSB = 0x27;
const uint8_t QUAT_ZDATA_LSB = 0x26;
const uint8_t QUAT_YDATA_MSB = 0x25;
const uint8_t QUAT_YDATA_LSB = 0x24;
const uint8_t QUAT_XDATA_MSB = 0x23;
const uint8_t QUAT_XDATA_LSB = 0x22;
const uint8_t QUAT_WDATA_MSB = 0x21;
const uint8_t QUAT_WDATA_LSB = 0x20;

//Pitch, roll, heading data
const uint8_t EULER_PITCH_MSB = 0x1F;
const uint8_t EULER_PITCH_LSB = 0x1E;
const uint8_t EULER_ROLL_MSB = 0x1D;
const uint8_t EULER_ROLL_LSB = 0x1C;
const uint8_t EULER_HEADING_MSB = 0x1B;
const uint8_t EULER_HEADING_LSB = 0x1A;

//Gyro data
const uint8_t GYRO_ZDATA_MSB = 0x19;
const uint8_t GYRO_ZDATA_LSB = 0x18;
const uint8_t GYRO_YDATA_MSB = 0x17;
const uint8_t GYRO_YDATA_LSB = 0x16;
const uint8_t GYRO_XDATA_MSB = 0x15;
const uint8_t GYRO_XDATA_LSB = 0x14;

//Mag data
const uint8_t MAG_ZDATA_MSB = 0x13;
const uint8_t MAG_ZDATA_LSB = 0x12;
const uint8_t MAG_YDATA_MSB = 0x11;
const uint8_t MAG_YDATA_LSB = 0x10;
const uint8_t MAG_XDATA_MSB = 0x0F;
const uint8_t MAG_XDATA_LSB = 0x0E;

//Accel data
const uint8_t ACCEL_ZDATA_MSB = 0x0D;
const uint8_t ACCEL_ZDATA_LSB = 0x0C;
const uint8_t ACCEL_YDATA_MSB = 0x0B;
const uint8_t ACCEL_YDATA_LSB = 0x0A;
const uint8_t ACCEL_XDATA_MSB = 0x09;
const uint8_t ACCEL_XDATA_LSB = 0x08;

//ID registers, pa
const uint8_t PAGE_ID = 0x07;
const uint8_t BOOTLOADER_ID = 0x06;
const uint8_t SW_REVISION_MSB = 0x05; //0x03 SW major revision
const uint8_t SW_REVISION_LSB = 0x04; //0x08 SW minor revision
const uint8_t GYRO_ID = 0x03; //0x0F default
const uint8_t MAG_ID = 0x02; //0x32 default
const uint8_t ACCEL_ID = 0x01; //0xFB default
const uint8_t CHIP_ID = 0x00; //0xA0 default
};

//These are page 1 registers
////////////////////////////

enum Page1 {
//BNO055 Unique ID range
const uint8_t UNIQUE_ID_START = 0x50;
const uint8_t UNIQUE_ID_END = 0x5F;

//Gyro interrupt settings
const uint8_t GYRO_ANYMOTION_SETUP = 0x1F;
const uint8_t GYRO_ANYMOTION_THRESHOLD = 0x1E;
const uint8_t GYRO_ZHIGHRATE_DURATION = 0x1D;
const uint8_t GYRO_ZHIGHRATE_THRESHOLD = 0x1C;
const uint8_t GYRO_YHIGHRATE_DURATION = 0x1B;
const uint8_t GYRO_YHIGHRATE_THRESHOLD = 0x1A;
const uint8_t GYRO_XHIGHRATE_DURATION = 0x19;
const uint8_t GYRO_XHIGHRATE_THRESHOLD = 0x18;
const uint8_t GYRO_INTERRUPT_SETUP = 0x17;

//Accel interrupt settings
const uint8_t ACCEL_NOMOTION_SETUP = 0x16;
const uint8_t ACCEL_NOMOTION_THRESHOLD = 0x15;
const uint8_t ACCEL_HIGHG_THRESHOLD = 0x14;
const uint8_t ACCEL_HIGHG_DURATION = 0x13;
const uint8_t ACCEL_INTERRUPT_SETTINGS = 0x12;
const uint8_t ACCEL_ANYMOTION_THRESHOLD = 0x11;

//CONFIG REGISTERS
const uint8_t INTERRUPT_ENABLE = 0x10;
const uint8_t INTERRUPT_MASK = 0x0F;
const uint8_t GYRO_SLEEP_CONFIG = 0x0D;
const uint8_t ACCEL_SLEEP_CONFIG = 0x0C;
const uint8_t GYRO_CONFIG_1 = 0x0B;
const uint8_t GYRO_CONFIG_0 = 0x0A;
const uint8_t MAG_CONFIG = 0x09;
const uint8_t ACCEL_CONFIG = 0x08;

//Page ID, write to switch
const uint8_t PAGE_ID = 0x07;
}; */
