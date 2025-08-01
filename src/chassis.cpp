#include <robomaster/chassis.hpp>

namespace robomaster
{

    attitude::attitude(package& p)
    {
        p >> yaw >> pitch >> roll;
    }

    wheel_encoders::wheel_encoders(package& p)
    {
        p >> rpm[0] >> rpm[1] >> rpm[2] >> rpm[3];
        p >> enc[0] >> enc[1] >> enc[2] >> enc[3];
        p >> timer[0] >> timer[1] >> timer[2] >> timer[3];
        p >> state[0] >> state[1] >> state[2] >> state[3];
    }

    imu::imu(package& p)
    {
        p >> acc_x >> acc_y >> acc_z;
        p >> gyr_x >> gyr_y >> gyr_z;
    }

    battery::battery(package& p)
    {
        p >> adc_val >> temperature >> current >> percent;
    }

    velocity::velocity(package& p)
    {
        p >> vgx >> vgy >> vgz;
        p >> vbx >> vby >> vbz;
    }

    chassis::chassis(std::iostream& io) : io_{io} {}

    void chassis::send_heartbeat()
    {
        package p_heartbeat
        {
            0x09, 0x03, 0x3f, 0x60, false, false,
            {
                0x00, 0x04, 0x20, 0x00, 0x01, 0x00, 0x40, 0x00,
                0x02, 0x10, 0x04, 0x03, 0x00, 0x04
            }
        };
        p_heartbeat.write_to(io_);
    }

    void chassis::send_workmode(uint8_t mode)
    {
        package pkg{0x09, 0xC3, 0x3f, 0x19, false, false};
        pkg << mode;
        pkg.write_to(io_);
    }

    void chassis::send_wheel_speed(int16_t w1, int16_t w2, int16_t w3, int16_t w4)
    {
        // w1 fr, w2 fl, w3 bl, w4 br
        package pkg{0xC9, 0xC3, 0x3f, 0x20, false, false};
        pkg << w1;
        pkg << static_cast<int16_t>(-w2);
        pkg << static_cast<int16_t>(-w3);
        pkg << w4;
        pkg.write_to(io_);
    }

    void chassis::send_speed(float vx, float vy, float omega)
    {
        package pkg{0xC9, 0xC3, 0x3f, 0x21, false, false};
        pkg << vx << vy << omega;
        pkg.write_to(io_);
    }

} // namespace robomaster
