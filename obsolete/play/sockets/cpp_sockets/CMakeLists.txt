cmake_minimum_required (VERSION 2.6)
project (socket_server)
add_executable(socket_server socket_server.cpp)
set_property(TARGET socket_server PROPERTY CXX_STANDARD 14)

