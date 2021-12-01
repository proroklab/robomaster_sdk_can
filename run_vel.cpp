#include <iostream>

#include <can_streambuf.hpp>
#include <chassis.hpp>


#include <chrono>
#include <thread>

#include <deque>

#include <iomanip>

int main(int, char**)
{
    auto can = can_streambuf("can0", 0x201);
    std::iostream io(&can);
    robomaster::command::chassis chassis(io);

    chassis.send_workmode(1);

    while (true)
    {
        chassis.send_heartbeat();
        chassis.send_wheel_speed(30, 0, 0, 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return 0;
}

