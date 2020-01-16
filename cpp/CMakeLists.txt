cmake_minimum_required(VERSION 2.8.2)
set(CMAKE_CXX_STANDARD 14)
#add_subdirectory(unit-tests)
add_definitions(-D$ENV{ROBOT}_car)

message("robot set to $ENV{ROBOT}")

include_directories(/usr/local/include/opencv4)
link_directories(/usr/local/lib)

set(CPP_FILES 
    ackerman.cpp
    camera.cpp
    car-controller.cpp
    car.cpp
    config.cpp
    diagnostics.cpp
    driver.cpp
    dynamics.cpp
    fake-car.cpp
    file-names.cpp
    frame-grabber.cpp
    geometry.cpp
    image-utils.cpp
    kalman.cpp
    lidar.cpp
    logger.cpp
    lookup-table.cpp
    main.cpp
    pid.cpp
    route.cpp
    run-settings.cpp
    socket-server.cpp
    speedometer.cpp
    stereo-camera.cpp
    string-utils.cpp
    system.cpp
    tracker.cpp
    usb.cpp
    web-server.cpp
    work-queue.cpp
)

# use a generator expression to silence a spurious warning from gcc

find_package(OpenCV REQUIRED)
add_executable(car-service ${CPP_FILES})
target_compile_options(car-service PRIVATE -Wno-psabi)
target_link_libraries(car-service ${OpenCV_LIBS} pthread)
