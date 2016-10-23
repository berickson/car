win32 {
    HOMEDIR += $$(USERPROFILE)
}
else {
    HOMEDIR += $$(HOME)
}

INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoavr/cores/arduino"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoavr/variants/standard"
INCLUDEPATH += "$${HOMEDIR}/car/teensy-play"
INCLUDEPATH += "$${HOMEDIR}/.platformio/lib/I2Cdevlib-Core_ID11"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoavr/libraries/Bridge/src"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoavr/libraries/EEPROM/src"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoavr/libraries/Esplora/src"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoavr/libraries/Ethernet/src"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoavr/libraries/Firmata"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoavr/libraries/Firmata/utility"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoavr/libraries/GSM/src"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoavr/libraries/HID/src"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoavr/libraries/Keyboard/src"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoavr/libraries/LiquidCrystal/src"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoavr/libraries/Mouse/src"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoavr/libraries/RobotIRremote/src"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoavr/libraries/Robot_Control/src"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoavr/libraries/Robot_Motor/src"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoavr/libraries/SD/src"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoavr/libraries/SPI/src"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoavr/libraries/Scheduler/src"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoavr/libraries/Servo/src"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoavr/libraries/SoftwareSerial/src"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoavr/libraries/SpacebrewYun/src"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoavr/libraries/Stepper/src"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoavr/libraries/TFT/src"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoavr/libraries/Temboo/src"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoavr/libraries/USBHost/src"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoavr/libraries/WiFi/src"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoavr/libraries/Wire/src"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/toolchain-atmelavr/avr/include"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/toolchain-atmelavr/lib/gcc/avr/4.8.1/include-fixed"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/toolchain-atmelavr/lib/gcc/avr/4.8.1/include"

DEFINES += "F_CPU=16000000L"
DEFINES += "PLATFORMIO=030001"
DEFINES += "ARDUINO_ARCH_AVR"
DEFINES += "ARDUINO_AVR_UNO"
DEFINES += "ARDUINO=10608"
DEFINES += "__AVR_ATmega328P__"

OTHER_FILES += platformio.ini

SOURCES += platformio.ini
SOURCES += .travis.yml
SOURCES += teensy-play.pro.user
SOURCES += teensy-play.pro
SOURCES += .platformio.ini.swp
SOURCES += .gitignore
SOURCES += .pioenvs/structure.hash
SOURCES += .pioenvs/do-not-modify-files-here.url
SOURCES += lib/readme.txt
SOURCES += src/Mpu9150.cpp
HEADERS += src/Mpu9150.h
SOURCES += src/Statistics.cpp
SOURCES += src/Logger.cpp
HEADERS += src/vector_math.h
SOURCES += src/teensy-play.cpp
HEADERS += src/Logger.h
HEADERS += src/helper_3dmath.h
HEADERS += src/Statistics.h
SOURCES += Editing bke-arduino_library.json at master · berickson_bke-arduino_files/frameworks-670aaf67517f485d1cc3d32a9303f11b30559d2d8b6bf839bf0199b83ad50468.css
SOURCES += Editing bke-arduino_library.json at master · berickson_bke-arduino_files/583067
SOURCES += Editing bke-arduino_library.json at master · berickson_bke-arduino_files/editor-098d693d641a759f41c2443c2d307955f3eeedab631613850686fa196b63fd47.js
SOURCES += Editing bke-arduino_library.json at master · berickson_bke-arduino_files/javascript.js
SOURCES += Editing bke-arduino_library.json at master · berickson_bke-arduino_files/github-92f407d1e778dafd05b0c49cf3a0018d2db470807dabea8eff08954ab1689f7e.js
SOURCES += Editing bke-arduino_library.json at master · berickson_bke-arduino_files/583067(1)
SOURCES += Editing bke-arduino_library.json at master · berickson_bke-arduino_files/github-2d1b79a5296eab9613c29bbac983651835f5ea62fb87eb9b860b7240dd28d934.css
SOURCES += Editing bke-arduino_library.json at master · berickson_bke-arduino_files/frameworks-33b22a6d8d4e7a6df6f3868fc101644ec8b92539c1d828b23b34dff4b2f613f7.js
