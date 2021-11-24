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
#include <robomaster_protocol.hpp>


#include <chrono>
#include <thread>

#include <iomanip>

int main(int, char **)
{
    auto can = can_streambuf("can0", 0x202);
    std::cout << "INIT" << std::endl;
    //auto can2 = can_streambuf("can0", 0x202);
    std::ostream out(&can);
    std::istream is(&can);
    std::vector<uint8_t> data{0xA0, 0x48, 0x08, 0x01};
    robomaster::package p_out{0x0309, data};
    
    robomaster::package p_in{};
    while(true) {
        is >> p_in;
        std::cout << "id " << std::hex << p_in.get_id() << ", cnt " << p_in.get_count();
        for (const auto& val : p_in.get_data())
        {
            std::cout << " " << std::hex << static_cast<int>(val);
        }
        std::cout << std::endl;
    
        //out << p_out;

        //std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}

