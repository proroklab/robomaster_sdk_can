#include <iostream>
#include <chrono>
#include <thread>

#include <robomaster/can_streambuf.hpp>
#include <robomaster/gimbal.hpp>

int main(int, char**)
{
    auto can = robomaster::can_streambuf("can0", 0x200);
    std::iostream io(&can);
    robomaster::gimbal gimbal(io);

    gimbal.send_workmode(1);
    gimbal.send_angles(100, 200);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    gimbal.recenter();

    return 0;
}
