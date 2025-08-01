#include <iostream>
#include <chrono>
#include <thread>

#include <robomaster/can_streambuf.hpp>
#include <robomaster/chassis.hpp>

int main(int, char**)
{
    auto can = robomaster::can_streambuf("can0", 0x201);
    std::iostream io(&can);
    robomaster::chassis chassis(io);

    chassis.send_workmode(1);

    while (true)
    {
        chassis.send_heartbeat();
        chassis.send_wheel_speed(30, 0, 0, 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return 0;
}
