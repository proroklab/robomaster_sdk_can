cmake_minimum_required(VERSION 3.16)
project(robomaster_comm VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Threads REQUIRED)

# Create the main library
add_library(robomaster_comm STATIC
        src/crc.cpp
        src/protocol.cpp
        src/dds.cpp
        src/chassis.cpp
        src/gimbal.cpp
        src/led.cpp
        src/can_streambuf.cpp
        )

# Set include directories
target_include_directories(robomaster_comm
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        )

# Link libraries
target_link_libraries(robomaster_comm
        PUBLIC Threads::Threads
        )

# Compiler options
target_compile_options(robomaster_comm PRIVATE
        -Wall -Wextra -Wpedantic
        )

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(robomaster_comm PRIVATE DEBUG_MODE=1)
    target_compile_options(robomaster_comm PRIVATE -g)
endif()

# Examples
add_subdirectory(examples)

# Installation
install(TARGETS robomaster_comm
        EXPORT robomaster_commTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
        )

install(DIRECTORY include/
        DESTINATION include
        )

install(EXPORT robomaster_commTargets
        FILE robomaster_commTargets.cmake
        NAMESPACE robomaster::
        DESTINATION lib/cmake/robomaster_comm
        )

# Generate config files
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        robomaster_commConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/robomaster_commConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/robomaster_commConfig.cmake
        INSTALL_DESTINATION lib/cmake/robomaster_comm
)

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/robomaster_commConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/robomaster_commConfigVersion.cmake
        DESTINATION lib/cmake/robomaster_comm
        )
