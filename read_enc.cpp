#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <iostream>
#include <fcntl.h>

#include <chrono>
#include <thread>

#include <can_streambuf.hpp>
#include <crc.hpp>
#include <protocol.hpp>
#include <dds.hpp>
#include <chassis.hpp>

using robomaster::dds::metadata;
using robomaster::dds::attitude;
using robomaster::dds::wheel_encoders;
using robomaster::dds::imu;
using robomaster::dds::battery;
using robomaster::dds::velocity;

void cb_attitude(const metadata&, const attitude& attitude)
{
    std::cout << "Attitude " << attitude.roll << " "  << attitude.pitch << " " << attitude.yaw << "\n";
}

void cb_wheel_enc(const metadata&, const wheel_encoders& wheel_encoders)
{
    std::cout << "ENC RPM " << static_cast<int>(wheel_encoders.rpm[0]) << "\n";
}

void cb_imu(const metadata&, const imu& imu)
{
    std::cout << "GYRO " << imu.gyr_x << "\n";
}

void cb_bat(const metadata&, const battery& battery)
{
    std::cout << "BAT " << static_cast<int>(battery.percent) << "\n";
}

void cb_vel(const metadata&, const velocity& velocity)
{
    std::cout << "Vel " << velocity.vgy << "\n";
}

class test
{
public:
    test() {}

    void cb_vel(const metadata& meta, const attitude& attitude, const battery& battery)
    {
        std::cout << "t " << meta.time_ns << "Vel cls " << attitude.yaw << " bat " << static_cast<int>(battery.percent) << std::endl;
    }
};

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

int main(int, char**)
{
    auto can_in = can_streambuf("can0", 0x202);
    auto can_cfg = can_streambuf("can0", 0x201);
    std::iostream in(&can_in);
    std::iostream out(&can_cfg);
    robomaster::dds::dds dds(in, out);

    test tst{};

    dds.subscribe(std::function<void(const metadata&, const attitude&, const battery&)>(std::bind(&test::cb_vel, tst, _1, _2, _3)), 20);
    dds.subscribe(std::function<void(const metadata&, const attitude&)>(cb_attitude), 20);
    dds.subscribe(std::function<void(const metadata&, const wheel_encoders&)>(cb_wheel_enc), 20);
    dds.subscribe(std::function<void(const metadata&, const imu&)>(cb_imu), 20);
    dds.subscribe(std::function<void(const metadata&, const velocity&)>(cb_vel), 20);
    dds.subscribe(std::function<void(const metadata&, const battery&)>(cb_bat), 1);
    out.flush();

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}

