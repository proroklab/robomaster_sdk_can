cmake_minimum_required(VERSION 3.16)
project(robomaster_comm)

set (CMAKE_CXX_STANDARD 14)

find_package(Threads REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/src)

add_executable(run_vel run_vel.cpp src/can_streambuf.cpp src/protocol.cpp src/crc.cpp)
add_executable(read_enc read_enc.cpp src/can_streambuf.cpp src/protocol.cpp src/crc.cpp src/chassis.cpp)

target_link_libraries(read_enc PRIVATE Threads::Threads)

target_compile_options(run_vel PRIVATE -Wall -Wextra -Wpedantic)
target_compile_options(read_enc PRIVATE -Wall -Wextra -Wpedantic)

