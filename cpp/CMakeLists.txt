cmake_minimum_required(VERSION 2.8.2)
set(CMAKE_CXX_STANDARD 14)
#add_subdirectory(unit-tests)

include_directories(/usr/local/include/opencv4)
link_directories(/usr/local/lib)

set(CPP_FILES 
    ackerman.cpp
    camera.cpp
    car_controller.cpp
    car.cpp
    config.cpp
    diagnostics.cpp
    driver.cpp
    dynamics.cpp
    fake_car.cpp
    file_names.cpp
    frame_grabber.cpp
    geometry.cpp
    image_utils.cpp
    kalman.cpp
    lidar.cpp
    logger.cpp
    lookup_table.cpp
    main.cpp
    pid.cpp
    route.cpp
    run_settings.cpp
    socket_server.cpp
    speedometer.cpp
    stereo_camera.cpp
    string_utils.cpp
    system.cpp
    tracker.cpp
    usb.cpp
    work_queue.cpp
)

find_package(OpenCV REQUIRED)
add_executable(car-service ${CPP_FILES})
target_link_libraries(car-service ${OpenCV_LIBS} pthread)
