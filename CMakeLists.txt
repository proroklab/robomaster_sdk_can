cmake_minimum_required(VERSION 3.16)
project(robomaster_comm)

set (CMAKE_CXX_STANDARD 14)

find_package(Threads REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/inc)

#add_executable(can_demo can_demo.cpp inc/can_streambuf.hpp inc/robomaster_protocol.hpp inc/crc.hpp)
add_executable(run_vel run_vel.cpp inc/can_streambuf.hpp inc/robomaster_protocol.hpp inc/crc.hpp)
add_executable(read_ctrl read_ctrl.cpp inc/can_streambuf.hpp inc/robomaster_protocol.hpp inc/crc.hpp inc/chassis.hpp)
add_executable(read_enc read_enc.cpp inc/can_streambuf.hpp inc/robomaster_protocol.hpp inc/crc.hpp inc/chassis.hpp)

target_link_libraries(read_enc PRIVATE Threads::Threads)

#target_compile_options(can_demo PRIVATE -Wall -Wextra -Wpedantic)
target_compile_options(run_vel PRIVATE -Wall -Wextra -Wpedantic)
target_compile_options(read_ctrl PRIVATE -Wall -Wextra -Wpedantic)
target_compile_options(read_enc PRIVATE -Wall -Wextra -Wpedantic)

