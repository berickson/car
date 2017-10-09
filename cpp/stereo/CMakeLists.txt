cmake_minimum_required (VERSION 2.8.11)
project (stereo)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package( OpenCV REQUIRED)
find_package( OpenGL )

if(OPENGL_FOUND)
  include_directories( ${OpenCV_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIRS} )
else()
  include_directories( ${OpenCV_INCLUDE_DIRS} )
endif()

link_directories("/usr/local/lib")
include_directories("/home/brian/eigen-3.3.4/Eigen /usr/local/lib")
add_executable (stereo stereo.cpp)
target_link_libraries(stereo ${OpenCV_LIBS})
