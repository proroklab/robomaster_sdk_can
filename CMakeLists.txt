cmake_minimum_required(VERSION 3.16)
project(robomaster_comm)

set (CMAKE_CXX_STANDARD 14)

include_directories(${PROJECT_SOURCE_DIR}/inc)

add_executable(can_demo can_demo.cpp inc/can_streambuf.hpp inc/robomaster_protocol.hpp inc/crc.hpp)

target_compile_options(can_demo PRIVATE -Wall -Wextra -Wpedantic)
