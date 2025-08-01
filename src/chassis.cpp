#pragma once

#include <protocol.cpp>
#include <dds.cpp>

namespace robomaster
{
namespace dds
{
struct attitude
{
    attitude(package& p)
    {
        p >> yaw >> pitch >> roll;
    }

    float yaw, pitch, roll;
};
template <> constexpr std::array<uint8_t, 8> get_uid<attitude>()
{
    return {0x42, 0xee, 0x13, 0x1d, 0x03, 0x00, 0x02, 0x00};
}

struct wheel_encoders
{
    wheel_encoders(package& p)
    {
        p >> rpm[0] >> rpm[1] >> rpm[2] >> rpm[3];
        p >> enc[0] >> enc[1] >> enc[2] >> enc[3];
        p >> timer[0] >> timer[1] >> timer[2] >> timer[3];
        p >> state[0] >> state[1] >> state[2] >> state[3];
    }

    int16_t rpm[4];
    uint16_t enc[4];
    uint32_t timer[4];
    uint8_t state[4];
};
template <> constexpr std::array<uint8_t, 8> get_uid<wheel_encoders>()
{
    return {0x09, 0xa3, 0x26, 0xe2, 0x03, 0x00, 0x02, 0x00};
}

struct imu
{
    imu(package& p)
    {
        p >> acc_x >> acc_y >> acc_z;
        p >> gyr_x >> gyr_y >> gyr_z;
    }

    float acc_x, acc_y, acc_z;
    float gyr_x, gyr_y, gyr_z;
};
template <> constexpr std::array<uint8_t, 8> get_uid<imu>()
{
    return {0xf4, 0x1d, 0x1c, 0xdc, 0x03, 0x00, 0x02, 0x00};
}

struct battery
{
    battery(package& p)
    {
        p >> adc_val >> temperature >> current >> percent;
    }

    uint16_t adc_val;
    int16_t temperature;
    int32_t current;
    uint8_t percent;
};
template <> constexpr std::array<uint8_t, 8> get_uid<battery>()
{
    return {0xfb, 0xdc, 0xf5, 0xd7, 0x03, 0x00, 0x02, 0x00};
}

struct velocity
{
    velocity(package& p)
    {
        p >> vgx >> vgy >> vgz;
        p >> vbx >> vby >> vbz;
    }
    float vgx, vgy, vgz;
    float vbx, vby, vbz;
};
template <> constexpr std::array<uint8_t, 8> get_uid<velocity>()
{
    return {0x66, 0x3e, 0x3e, 0x4c, 0x03, 0x00, 0x02, 0x00};
}

}

namespace command
{
class chassis
{
public:
    chassis(std::iostream& io)
        : _io{io}
    {}

    void send_heartbeat()
    {
        robomaster::package p_heartbeat
        {
            0x09, 0x03, 0x3f, 0x60, false, false,
            {
                0x00, 0x04, 0x20, 0x00, 0x01, 0x00, 0x40, 0x00,
                0x02, 0x10, 0x04, 0x03, 0x00, 0x04
            }
        };
        p_heartbeat.write_to(_io);
    }

    void send_workmode(uint8_t mode)
    {
        robomaster::package pkg{0x09, 0xC3, 0x3f, 0x19, false, false};
        pkg << mode;
        pkg.write_to(_io);
    }

    void send_wheel_speed(int16_t w1, int16_t w2, int16_t w3, int16_t w4)
    {
        // w1 fr, w2 fl, w3 bl, w4 br
        robomaster::package pkg{0xC9, 0xC3, 0x3f, 0x20, false, false};
        pkg << w1;
        pkg << static_cast<int16_t>(-w2);
        pkg << static_cast<int16_t>(-w3);
        pkg << w4;
        pkg.write_to(_io);
    }

    void send_speed(float vx, float vy, float omega)
    {
        robomaster::package pkg{0xC9, 0xC3, 0x3f, 0x21, false, false};
        pkg << vx << vy << omega;
        pkg.write_to(_io);
    }

private:
    std::iostream& _io;
};

}
}

