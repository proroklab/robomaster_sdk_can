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

#include <CANManager.hpp>


#include <chrono>
#include <thread>


int main(int, char **)
{
    auto can = CANManager("can0", 0x202);
    std::cout << "INIT" << std::endl;
    //auto can2 = CANManager("can0", 0x202);
    std::ostream out(&can);
    std::istream is(&can);
    while(true) {
        //out << static_cast<char>(0x55);
        char c;
        is.read(&c, 1);
        std::cout << std::hex << c << std::dec;
        std::cout << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}

