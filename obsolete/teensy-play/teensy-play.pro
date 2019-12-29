win32 {
    HOMEDIR += $$(USERPROFILE)
}
else {
    HOMEDIR += $$(HOME)
}

INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/cores/teensy3"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/Wire"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/Wire/utility"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/Servo"
INCLUDEPATH += "$${HOMEDIR}/car/teensy"
INCLUDEPATH += "$${HOMEDIR}/.platformio/lib/I2Cdevlib_Core_ID11"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/ADC"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/AccelStepper"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/Adafruit_CC3000"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/Adafruit_CC3000/utility"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/Adafruit_GFX"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/Adafruit_ILI9340"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/Adafruit_ILI9341"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/Adafruit_NeoPixel"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/Adafruit_RA8875"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/Adafruit_SSD1306"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/Adafruit_ST7735"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/Adafruit_STMPE610"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/Adafruit_VS1053"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/Adafruit_nRF8001"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/Adafruit_nRF8001/utility"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/AltSoftSerial"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/Artnet"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/Audio"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/Audio/utility"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/Bounce"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/Bounce2"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/CapacitiveSensor"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/DS1307RTC"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/DmxSimple"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/DogLcd"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/EEPROM"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/Encoder"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/Encoder/utility"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/Entropy"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/Ethernet"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/FastCRC"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/FastLED"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/FlexCAN"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/FlexiTimer2"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/FreqCount"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/FreqMeasure"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/FreqMeasureMulti"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/FrequencyTimer2"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/ILI9341_t3"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/IRremote"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/Keypad"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/Keypad/utility"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/LedControl/src"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/LedDisplay"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/LiquidCrystal"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/LiquidCrystalFast"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/LowPower"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/MFRC522"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/MIDI"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/Metro"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/MsTimer2"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/NXPMotionSense"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/NXPMotionSense/utility"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/NewPing"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/OSC"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/OctoWS2811"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/OneWire"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/PS2Keyboard"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/PS2Keyboard/utility"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/PWMServo"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/Ping"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/PulsePosition"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/RA8875"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/RadioHead"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/SD"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/SD/utility"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/SPI"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/SPIFlash"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/ST7565"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/SerialFlash"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/Servo"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/ShiftPWM"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/Snooze"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/Snooze/utility"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/SoftPWM"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/SoftwareSerial"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/TFT_ILI9163C"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/Talkie"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/Time"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/TimeAlarms"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/TimerOne"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/TimerThree"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/TinyGPS"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/Tlc5940"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/TouchScreen"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/UTFT"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/VirtualWire"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/Wire"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/Wire/utility"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/XBee"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/XPT2046_Touchscreen"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/i2c_t3"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/ks0108"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/openGLCD"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/ssd1351"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoteensy/libraries/x10"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/toolchain-gccarmnoneeabi/arm-none-eabi/include"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/toolchain-gccarmnoneeabi/lib/gcc/arm-none-eabi/4.8.4/include-fixed"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/toolchain-gccarmnoneeabi/lib/gcc/arm-none-eabi/4.8.4/include"
INCLUDEPATH += "$${HOMEDIR}/.piolibdeps/bke-arduino"

DEFINES += "F_CPU=72000000L"
DEFINES += "LAYOUT_US_ENGLISH"
DEFINES += "PLATFORMIO=030001"
DEFINES += "__MK20DX256__"
DEFINES += "TEENSY31"
DEFINES += "USB_SERIAL"
DEFINES += "ARDUINO=10600"
DEFINES += "TEENSYDUINO=130"

OTHER_FILES += platformio.ini

SOURCES += platformio.ini \
    Mpu9150.cpp \
    MPU6050.cpp \
    lib/MPU6050/MPU6050.cpp \
    src/teensy-play.cpp \
    .piolibdeps/SparkFun MPU-9250 9 DOF IMU Breakout_ID944/src/MPU9250.cpp \
    .piolibdeps/SparkFun MPU-9250 9 DOF IMU Breakout_ID944/src/quaternionFilters.cpp \
    .piolibdeps/SparkFun MPU-9250 9 DOF IMU Breakout_ID944/src/MPU9250.cpp \
    .piolibdeps/SparkFun MPU-9250 9 DOF IMU Breakout_ID944/src/quaternionFilters.cpp \
    .piolibdeps/SparkFun MPU-9250 9 DOF IMU Breakout_ID944/src/MPU9250.cpp \
    .piolibdeps/SparkFun MPU-9250 9 DOF IMU Breakout_ID944/src/quaternionFilters.cpp \
    .piolibdeps/SparkFun MPU-9250 9 DOF IMU Breakout_ID944/src/MPU9250.cpp \
    .piolibdeps/SparkFun MPU-9250 9 DOF IMU Breakout_ID944/src/quaternionFilters.cpp \
    .piolibdeps/SparkFun MPU-9250 9 DOF IMU Breakout_ID944/src/MPU9250.cpp \
    .piolibdeps/SparkFun MPU-9250 9 DOF IMU Breakout_ID944/src/quaternionFilters.cpp \
    .piolibdeps/I2Cdevlib-Core_ID11/I2Cdev.cpp \
    .piolibdeps/I2Cdevlib-MPU9150_ID113/MPU9150.cpp \
    .piolibdeps/SparkFun MPU-9250 9 DOF IMU Breakout_ID944/src/MPU9250.cpp \
    .piolibdeps/SparkFun MPU-9250 9 DOF IMU Breakout_ID944/src/quaternionFilters.cpp \
    src/Statistics.cpp \
    .piolibdeps/bke-arduino/play.cpp \
    .piolibdeps/bke-arduino/Logger.cpp \
    .piolibdeps/bke-arduino/Mpu9150.cpp \
    .piolibdeps/bke-arduino/Statistics.cpp
HEADERS += Car.h \
    teensy-play.ino \
    Mpu9150.h \
    lib/I2Cdev/I2Cdev.h \
    lib/MPU6050/Examples/MPU6050_raw/MPU6050_raw.ino \
    lib/MPU6050/Examples/MPU9150_raw/MPU9150_raw.ino \
    lib/MPU6050/helper_3dmath.h \
    lib/MPU6050/MPU6050.h \
    lib/MPU6050/MPU6050_6Axis_MotionApps20.h \
    lib/MPU6050/MPU6050_9Axis_MotionApps41.h \
    lib/MPU6050/vector_math.h \
    src/helper_3dmath.h \
    src/Logger.h \
    src/Mpu9150.h \
    src/vector_math.h \
    src/teensy-play.ino \
    .piolibdeps/I2Cdevlib-Core_ID11/I2Cdev.h \
    .piolibdeps/I2Cdevlib-MPU9150_ID113/examples/MPU9150_raw/MPU9150_raw.ino \
    .piolibdeps/I2Cdevlib-MPU9150_ID113/helper_3dmath.h \
    .piolibdeps/I2Cdevlib-MPU9150_ID113/MPU9150.h \
    .piolibdeps/I2Cdevlib-MPU9150_ID113/MPU9150_9Axis_MotionApps41.h \
    .piolibdeps/SparkFun MPU-9250 9 DOF IMU Breakout_ID944/examples/MPU9250BasicAHRS/MPU9250BasicAHRS.ino \
    .piolibdeps/SparkFun MPU-9250 9 DOF IMU Breakout_ID944/src/MPU9250.h \
    .piolibdeps/SparkFun MPU-9250 9 DOF IMU Breakout_ID944/src/quaternionFilters.h \
    .piolibdeps/SparkFun MPU-9250 9 DOF IMU Breakout_ID944/src/MPU9250.h \
    .piolibdeps/SparkFun MPU-9250 9 DOF IMU Breakout_ID944/src/quaternionFilters.h \
    .piolibdeps/SparkFun MPU-9250 9 DOF IMU Breakout_ID944/src/MPU9250.h \
    .piolibdeps/SparkFun MPU-9250 9 DOF IMU Breakout_ID944/src/quaternionFilters.h \
    .piolibdeps/SparkFun MPU-9250 9 DOF IMU Breakout_ID944/examples/MPU9250BasicAHRS/MPU9250BasicAHRS.ino \
    .piolibdeps/SparkFun MPU-9250 9 DOF IMU Breakout_ID944/src/MPU9250.h \
    .piolibdeps/SparkFun MPU-9250 9 DOF IMU Breakout_ID944/src/quaternionFilters.h \
    .piolibdeps/SparkFun MPU-9250 9 DOF IMU Breakout_ID944/examples/MPU9250BasicAHRS/MPU9250BasicAHRS.ino \
    .piolibdeps/SparkFun MPU-9250 9 DOF IMU Breakout_ID944/src/MPU9250.h \
    .piolibdeps/SparkFun MPU-9250 9 DOF IMU Breakout_ID944/src/quaternionFilters.h \
    .piolibdeps/SparkFun MPU-9250 9 DOF IMU Breakout_ID944/src/MPU9250.h \
    .piolibdeps/SparkFun MPU-9250 9 DOF IMU Breakout_ID944/src/MPU9250.h \
    .piolibdeps/SparkFun MPU-9250 9 DOF IMU Breakout_ID944/src/quaternionFilters.h \
    src/Mpu9250.h \
    .piolibdeps/bke-arduino/play.h \
    .piolibdeps/bke-arduino/Logger.h \
    .piolibdeps/bke-arduino/Statistics.h \
    .piolibdeps/bke-arduino/Mpu9150.h
SOURCES += EventQueue.cpp
HEADERS += Pid.h
HEADERS += ManualMode.h
SOURCES += FollowMode.cpp
SOURCES += ManualMode.cpp
SOURCES += RemoteMode.cpp
HEADERS += BNO055.h
SOURCES += BNO055.cpp
SOURCES += teensy.ino
HEADERS += EventQueue.h
SOURCES += Statistics.cpp
HEADERS += RemoteMode.h
SOURCES += Task.cpp
SOURCES += .travis.yml
SOURCES += Fsm.cpp
SOURCES += Untitled File
HEADERS += Beeper.h
SOURCES += Untitled File(1)
SOURCES += Logger.cpp
HEADERS += Task.h
SOURCES += Pid.cpp
HEADERS += Pins.h
HEADERS += Logger.h
SOURCES += Blinker.cpp
SOURCES += Sequence.cpp
HEADERS += CircleMode.h
HEADERS += Command.h
HEADERS += FollowMode.h
HEADERS += Sequence.h
SOURCES += CircleMode.cpp
HEADERS += QuadratureEncoder.h
SOURCES += .gitignore
HEADERS += Fsm.h
SOURCES += RxEvent.cpp
HEADERS += RxEvent.h
SOURCES += Beeper.cpp
SOURCES += Beep.cpp
SOURCES += Ping.cpp
HEADERS += RxEvents.h
HEADERS += CommandInterpreter.h
SOURCES += CommandInterpreter.cpp
SOURCES += RxEvents.cpp
SOURCES += Esc.cpp
HEADERS += Beep.h
HEADERS += PwmInput.h
SOURCES += upload
HEADERS += Esc.h
HEADERS += Statistics.h
HEADERS += Blinker.h
SOURCES += PwmInput.cpp
HEADERS += BNODEFS.h
HEADERS += Ping.h
SOURCES += .pioenvs/.sconsign.dblite
SOURCES += .pioenvs/structure.hash
SOURCES += .pioenvs/do-not-modify-files-here.url
SOURCES += .pioenvs/teensy31/libFrameworkArduino.a
SOURCES += .pioenvs/teensy31/firmware.hex
SOURCES += .pioenvs/teensy31/firmware.elf
SOURCES += .pioenvs/teensy31/FrameworkArduino/HardwareSerial5.o
SOURCES += .pioenvs/teensy31/FrameworkArduino/WString.o
SOURCES += .pioenvs/teensy31/FrameworkArduino/HardwareSerial1.o
SOURCES += .pioenvs/teensy31/FrameworkArduino/usb_mtp.o
SOURCES += .pioenvs/teensy31/FrameworkArduino/serial2.o
SOURCES += .pioenvs/teensy31/FrameworkArduino/HardwareSerial2.o
SOURCES += .pioenvs/teensy31/FrameworkArduino/memcpy-armv7m.o
SOURCES += .pioenvs/teensy31/FrameworkArduino/usb_rawhid.o
SOURCES += .pioenvs/teensy31/FrameworkArduino/serial4.o
SOURCES += .pioenvs/teensy31/FrameworkArduino/serial1.o
SOURCES += .pioenvs/teensy31/FrameworkArduino/usb_mouse.o
SOURCES += .pioenvs/teensy31/FrameworkArduino/usb_joystick.o
SOURCES += .pioenvs/teensy31/FrameworkArduino/AudioStream.o
SOURCES += .pioenvs/teensy31/FrameworkArduino/usb_desc.o
SOURCES += .pioenvs/teensy31/FrameworkArduino/IntervalTimer.o
SOURCES += .pioenvs/teensy31/FrameworkArduino/usb_inst.o
SOURCES += .pioenvs/teensy31/FrameworkArduino/HardwareSerial6.o
SOURCES += .pioenvs/teensy31/FrameworkArduino/usb_audio.o
SOURCES += .pioenvs/teensy31/FrameworkArduino/usb_serial.o
SOURCES += .pioenvs/teensy31/FrameworkArduino/HardwareSerial3.o
SOURCES += .pioenvs/teensy31/FrameworkArduino/eeprom.o
SOURCES += .pioenvs/teensy31/FrameworkArduino/nonstd.o
SOURCES += .pioenvs/teensy31/FrameworkArduino/keylayouts.o
SOURCES += .pioenvs/teensy31/FrameworkArduino/usb_flightsim.o
SOURCES += .pioenvs/teensy31/FrameworkArduino/usb_mem.o
SOURCES += .pioenvs/teensy31/FrameworkArduino/main.o
SOURCES += .pioenvs/teensy31/FrameworkArduino/yield.o
SOURCES += .pioenvs/teensy31/FrameworkArduino/analog.o
SOURCES += .pioenvs/teensy31/FrameworkArduino/new.o
SOURCES += .pioenvs/teensy31/FrameworkArduino/usb_seremu.o
SOURCES += .pioenvs/teensy31/FrameworkArduino/serial5.o
SOURCES += .pioenvs/teensy31/FrameworkArduino/IPAddress.o
SOURCES += .pioenvs/teensy31/FrameworkArduino/touch.o
SOURCES += .pioenvs/teensy31/FrameworkArduino/memset.o
SOURCES += .pioenvs/teensy31/FrameworkArduino/WMath.o
SOURCES += .pioenvs/teensy31/FrameworkArduino/usb_dev.o
SOURCES += .pioenvs/teensy31/FrameworkArduino/Print.o
SOURCES += .pioenvs/teensy31/FrameworkArduino/ser_print.o
SOURCES += .pioenvs/teensy31/FrameworkArduino/HardwareSerial4.o
SOURCES += .pioenvs/teensy31/FrameworkArduino/usb_touch.o
SOURCES += .pioenvs/teensy31/FrameworkArduino/math_helper.o
SOURCES += .pioenvs/teensy31/FrameworkArduino/DMAChannel.o
SOURCES += .pioenvs/teensy31/FrameworkArduino/Tone.o
SOURCES += .pioenvs/teensy31/FrameworkArduino/mk20dx128.o
SOURCES += .pioenvs/teensy31/FrameworkArduino/avr_emulation.o
SOURCES += .pioenvs/teensy31/FrameworkArduino/usb_keyboard.o
SOURCES += .pioenvs/teensy31/FrameworkArduino/Stream.o
SOURCES += .pioenvs/teensy31/FrameworkArduino/pins_teensy.o
SOURCES += .pioenvs/teensy31/FrameworkArduino/serial6.o
SOURCES += .pioenvs/teensy31/FrameworkArduino/serial3.o
SOURCES += .pioenvs/teensy31/FrameworkArduino/usb_midi.o
SOURCES += .pioenvs/teensy31/FrameworkArduino/serial6_lpuart.o
SOURCES += .pioenvs/teensy31/lib/libServo.a
SOURCES += .pioenvs/teensy31/lib/libWire.a
SOURCES += .pioenvs/teensy31/lib/Servo/Servo.o
SOURCES += .pioenvs/teensy31/lib/Wire/Wire.o
SOURCES += .pioenvs/teensy31/lib/Wire/utility/twi.o
SOURCES += .pioenvs/teensy31/src/Blinker.o
SOURCES += .pioenvs/teensy31/src/teensy.ino.o
SOURCES += .pioenvs/teensy31/src/Esc.o
SOURCES += .pioenvs/teensy31/src/PwmInput.o
SOURCES += .pioenvs/teensy31/src/RxEvent.o
SOURCES += .pioenvs/teensy31/src/Beep.o
SOURCES += .pioenvs/teensy31/src/Task.o
SOURCES += .pioenvs/teensy31/src/CommandInterpreter.o
SOURCES += .pioenvs/teensy31/src/Fsm.o
SOURCES += .pioenvs/teensy31/src/CircleMode.o
SOURCES += .pioenvs/teensy31/src/Sequence.o
SOURCES += .pioenvs/teensy31/src/BNO055.o
SOURCES += .pioenvs/teensy31/src/Ping.o
SOURCES += .pioenvs/teensy31/src/Pid.o
SOURCES += .pioenvs/teensy31/src/RxEvents.o
SOURCES += .pioenvs/teensy31/src/RemoteMode.o
SOURCES += .pioenvs/teensy31/src/Logger.o
SOURCES += .pioenvs/teensy31/src/ManualMode.o
SOURCES += .pioenvs/teensy31/src/EventQueue.o
SOURCES += .pioenvs/teensy31/src/Statistics.o
SOURCES += .pioenvs/teensy31/src/FollowMode.o
SOURCES += .pioenvs/teensy31/src/Beeper.o
SOURCES += lib/readme.txt
SOURCES += test/makefile
HEADERS += test/FakeTask.h
HEADERS += test/BeepTest.h
HEADERS += test/BeeperTest.h
SOURCES += test/Arduino.cpp
HEADERS += test/StatisticsTest.h
HEADERS += test/Arduino.h
SOURCES += test/test_output.txt
SOURCES += test/.gitignore
HEADERS += test/FsmTest.h
SOURCES += test/FakeTask.cpp
HEADERS += test/SequenceTest.h
SOURCES += test/main.cpp
HEADERS += test/PidTest.h
HEADERS += test/gtest/gtest_prod.h
HEADERS += test/gtest/gtest-printers.h
SOURCES += test/gtest/libgtest.a
HEADERS += test/gtest/gtest_pred_impl.h
HEADERS += test/gtest/gtest.h
HEADERS += test/gtest/gtest-message.h
HEADERS += test/gtest/gtest-param-test.h
SOURCES += test/gtest/gtest-param-test.h.pump
HEADERS += test/gtest/gtest-typed-test.h
HEADERS += test/gtest/gtest-test-part.h
HEADERS += test/gtest/gtest-death-test.h
HEADERS += test/gtest/gtest-spi.h
HEADERS += test/gtest/internal/gtest-tuple.h
HEADERS += test/gtest/internal/gtest-port.h
HEADERS += test/gtest/internal/gtest-string.h
HEADERS += test/gtest/internal/gtest-linked_ptr.h
HEADERS += test/gtest/internal/gtest-death-test-internal.h
SOURCES += test/gtest/internal/gtest-param-util-generated.h.pump
HEADERS += test/gtest/internal/gtest-param-util.h
SOURCES += test/gtest/internal/gtest-tuple.h.pump
SOURCES += test/gtest/internal/gtest-type-util.h.pump
HEADERS += test/gtest/internal/gtest-type-util.h
HEADERS += test/gtest/internal/gtest-port-arch.h
HEADERS += test/gtest/internal/gtest-param-util-generated.h
HEADERS += test/gtest/internal/gtest-filepath.h
HEADERS += test/gtest/internal/gtest-internal.h
HEADERS += test/gtest/internal/custom/gtest-port.h
HEADERS += test/gtest/internal/custom/gtest-printers.h
HEADERS += test/gtest/internal/custom/gtest.h

DISTFILES += \
    lib/MPU6050/Examples/MPU6050_DMP6/Processing/MPUTeapot.pde \
    lib/MPU6050/Examples/MPU6050_DMP6/MPU6050_DMP6.ino \
    lib/MPU6050/Examples/MPU9150_AHRS.ino \
    .piolibdeps/SparkFun MPU-9250 9 DOF IMU Breakout_ID944/keywords.txt \
    .piolibdeps/SparkFun MPU-9250 9 DOF IMU Breakout_ID944/library.properties \
    .piolibdeps/SparkFun MPU-9250 9 DOF IMU Breakout_ID944/README.md \
    .piolibdeps/SparkFun MPU-9250 9 DOF IMU Breakout_ID944/keywords.txt \
    .piolibdeps/SparkFun MPU-9250 9 DOF IMU Breakout_ID944/library.properties \
    .piolibdeps/SparkFun MPU-9250 9 DOF IMU Breakout_ID944/README.md \
    .piolibdeps/SparkFun MPU-9250 9 DOF IMU Breakout_ID944/keywords.txt \
    .piolibdeps/SparkFun MPU-9250 9 DOF IMU Breakout_ID944/library.properties \
    .piolibdeps/SparkFun MPU-9250 9 DOF IMU Breakout_ID944/README.md \
    .piolibdeps/bke-arduino/library.json \
    .piolibdeps/I2Cdevlib-Core_ID11/library.json \
    .piolibdeps/I2Cdevlib-MPU9150_ID113/library.json \
    .piolibdeps/I2Cdevlib-Core_ID11/keywords.txt