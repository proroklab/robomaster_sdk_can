#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <iostream>
#include <fcntl.h>

#include <can_streambuf.hpp>
#include <crc.hpp>
#include <robomaster_protocol.hpp>
#include <dds.hpp>

void cb_imu(const struct robomaster::dds::imu& imu)
{
    std::cout << "IMU " << imu.yaw << "\n";
}

void cb_wheel_enc(const struct robomaster::dds::wheel_encoders& wheel_encoders)
{
    std::cout << "ENC RPM " << static_cast<int>(wheel_encoders.rpm[0]) << "\n";
}

void cb_acc_gyro(const struct robomaster::dds::acc_gyro& acc_gyro)
{
    std::cout << "GYRO " << acc_gyro.gyr_x << "\n";
}

void cb_bat(const robomaster::dds::battery& battery)
{
    std::cout << "BAT " << static_cast<int>(battery.percent) << "\n";
}

void cb_vel(const robomaster::dds::velocity& velocity)
{
    std::cout << "Vel " << velocity.vbx << "\n";
}

int main(int, char**)
{
    auto can_in = can_streambuf("can0", 0x202);
    auto can_cfg = can_streambuf("can0", 0x201);
    std::iostream in(&can_in);
    std::iostream out(&can_cfg);
    robomaster::dds::dds dds(in, out);
    dds.subscribe(cb_imu, 20);
    dds.subscribe(cb_wheel_enc, 20);
    dds.subscribe(cb_acc_gyro, 20);
    dds.subscribe(cb_vel, 20);
    dds.subscribe(cb_bat, 1);
    out.flush();

    while (true)
    {
        dds.run();
    }

    return 0;
}

