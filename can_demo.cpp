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

int main(int, char **)
{
    auto can = CANManager("can0", 0x203);
    //auto can2 = CANManager("can0", 0x202);
    while(true) {
        can.Run();
        //can2.Run();
    }

    return 0;
}

