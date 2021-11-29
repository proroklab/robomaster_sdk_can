#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <poll.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#include <iostream>
#include <fcntl.h>

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

    while (true)
    {
        chassis.send_heartbeat();
        chassis.send_wheel_speed(30, 0, 0, 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return 0;
}

