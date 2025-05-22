#include <iostream>

#include <can_streambuf.hpp>
#include <gimbal.hpp>


#include <chrono>
#include <thread>

#include <deque>

#include <iomanip>


int main(int, char**)
{
    auto can = can_streambuf("can0", 0x200);
    std::iostream io(&can);
    robomaster::command::gimbal gimbal(io);

    gimbal.send_workmode(1);
    gimbal.send_angles(100,200);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    gimbal.recenter();

    return 0;
}
