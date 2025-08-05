# RoboMaster Communication Library

A C++ library for communicating with DJI RoboMaster robots over CAN bus.
This library was reverse-engineered from DJI RoboMaster S1 CAN communications and the
communication protocol implicitly documented in the [RoboMaster SDK](https://github.com/dji-sdk/RoboMaster-SDK).

This library was tested on Raspberry Pis with the [CAN hat](https://www.waveshare.com/wiki/RS485_CAN_HAT) as well
as on Nvidia Jetson Orin NX with the [AverMedia D131](https://www.avermedia.com/professional/product-detail/D131) carrier board.

## Building

```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

## Usage

### Basic Robot Control

```cpp
#include <robomaster/robomaster.hpp>

int main() {
    // Create CAN interface
    auto can = robomaster::can_streambuf("can0", 0x201);
    std::iostream io(&can);
    
    // Initialize chassis controller
    robomaster::command::chassis chassis(io);
    
    // Set work mode and send commands
    chassis.send_workmode(1);
    chassis.send_wheel_speed(100, 0, 0, 0);  // Move forward
    
    return 0;
}
```

### Sensor Data Subscription

```cpp
#include <robomaster/robomaster.hpp>

void on_imu_data(const robomaster::dds::metadata& meta, 
                 const robomaster::dds::imu& imu_data) {
    std::cout << "IMU: " << imu_data.gyr_x << ", " 
              << imu_data.gyr_y << ", " << imu_data.gyr_z << std::endl;
}

int main() {
    auto can_in = robomaster::can_streambuf("can0", 0x202);
    auto can_out = robomaster::can_streambuf("can0", 0x201);
    std::iostream in(&can_in);
    std::iostream out(&can_out);
    
    robomaster::dds::dds dds_client(in, out);
    
    // Subscribe to IMU data at 50Hz
    dds_client.subscribe(std::function<void(const robomaster::dds::metadata&, 
                                           const robomaster::dds::imu&)>(on_imu_data), 50);
    
    // Keep running
    std::this_thread::sleep_for(std::chrono::seconds(10));
    return 0;
}
```

## Available Sensor Types

- `attitude`: Roll, pitch, yaw
- `wheel_encoders`: RPM, encoder values, timers, states
- `imu`: Accelerometer and gyroscope data
- `battery`: Voltage, temperature, current, percentage
- `velocity`: Global and body frame velocities

## Examples

See the `examples/` directory for complete working examples:
- `run_vel.cpp`: Basic robot movement
- `read_enc.cpp`: Multi-sensor data subscription

## Using in Your Project

### With CMake

```cmake
find_package(robomaster_comm REQUIRED)
target_link_libraries(your_target robomaster::robomaster_comm)
```

### Manual Linking

```bash
g++ -std=c++17 your_code.cpp -lrobomaster_comm -lpthread
```

## Requirements

- C++17 compiler
- Linux with SocketCAN support
- CMake 3.16+
- pthread library

## Error Handling

The library uses exceptions for error handling:
- `robomaster::can_error`: CAN socket operations
- `robomaster::protocol_error`: Protocol parsing/validation
- Standard exceptions: Invalid arguments, system errors

## Thread Safety

The library is not thread-safe and requires mutexes when the same interface is accessed
by multiple threads. An example can be found in `examples/threading.cpp`.

