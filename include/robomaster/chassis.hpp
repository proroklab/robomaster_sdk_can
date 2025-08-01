#pragma once

#include <robomaster/protocol.hpp>
#include <robomaster/dds.hpp>
#include <iostream>
#include <array>

namespace robomaster
{

struct attitude
{
    attitude(package& p);
    float yaw, pitch, roll;
};

template <>
constexpr std::array<uint8_t, 8> get_uid<attitude>()
{
    return {0x42, 0xee, 0x13, 0x1d, 0x03, 0x00, 0x02, 0x00};
}

struct wheel_encoders
{
    wheel_encoders(package& p);
    int16_t rpm[4];
    uint16_t enc[4];
    uint32_t timer[4];
    uint8_t state[4];
};

template <>
constexpr std::array<uint8_t, 8> get_uid<wheel_encoders>()
{
    return {0x09, 0xa3, 0x26, 0xe2, 0x03, 0x00, 0x02, 0x00};
}

struct imu
{
    imu(package& p);
    float acc_x, acc_y, acc_z;
    float gyr_x, gyr_y, gyr_z;
};

template <>
constexpr std::array<uint8_t, 8> get_uid<imu>()
{
    return {0xf4, 0x1d, 0x1c, 0xdc, 0x03, 0x00, 0x02, 0x00};
}

struct battery
{
    battery(package& p);
    uint16_t adc_val;
    int16_t temperature;
    int32_t current;
    uint8_t percent;
};

template <>
constexpr std::array<uint8_t, 8> get_uid<battery>()
{
    return {0xfb, 0xdc, 0xf5, 0xd7, 0x03, 0x00, 0x02, 0x00};
}

struct velocity
{
    velocity(package& p);
    float vgx, vgy, vgz;
    float vbx, vby, vbz;
};

template <>
constexpr std::array<uint8_t, 8> get_uid<velocity>()
{
    return {0x66, 0x3e, 0x3e, 0x4c, 0x03, 0x00, 0x02, 0x00};
}

class chassis
{
public:
    explicit chassis(std::iostream& io);

    void send_heartbeat();
    void send_workmode(uint8_t mode);
    void send_wheel_speed(int16_t w1, int16_t w2, int16_t w3, int16_t w4);
    void send_speed(float vx, float vy, float omega);

private:
    std::iostream& io_;
};

} // namespace robomaster
