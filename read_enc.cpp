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
#include <crc.hpp>
#include <robomaster_protocol.hpp>
#include <dds.hpp>


#include <chrono>
#include <thread>

#include <deque>

#include <iomanip>

/*
    can 202

    0xb3, 0xf7, 0xe6, 0x47, 0x03, 0x00, 0x02, 0x00
    uint32_t w1_timer_ms, w2_timer_ms, w3_timer_ms, w4_timer_ms;
    p_in >> w1_timer_ms >> w2_timer_ms >> w3_timer_ms >> w4_timer_ms;
    uint8_t w1_state, w2_state, w3_state, w4_state;
    p_in >> w1_state >> w2_state >> w3_state >> w4_state;
    
    0x42, 0xee, 0x13, 0x1d, 0x03, 0x00, 0x02, 0x00
    uint32_t time_ms, time_ns;
    p_in >> time_ms >> time_ns;
    float yaw, pitch, roll;
    p_in >> yaw >> pitch >> roll;

    0x09, 0xa3, 0x26, 0xe2, 0x03, 0x00, 0x02, 0x00
    uint32_t time_ms, time_ns;
    p_in >> time_ms >> time_ns;
    int16_t w1_rpm, w2_rpm, w3_rpm, w4_rpm;
    p_in >> w1_rpm >> w2_rpm >> w3_rpm >> w4_rpm;
    uint16_t w1_enc, w2_enc, w3_enc, w4_enc;
    p_in >> w1_enc >> w2_enc >> w3_enc >> w4_enc;
    uint32_t w1_timer_ms, w2_timer_ms, w3_timer_ms, w4_timer_ms;
    p_in >> w1_timer_ms >> w2_timer_ms >> w3_timer_ms >> w4_timer_ms;
    uint8_t w1_state, w2_state, w3_state, w4_state;
    p_in >> w1_state >> w2_state >> w3_state >> w4_state;
            
    0xf4, 0x1d, 0x1c, 0xdc, 0x03, 0x00, 0x02, 0x00
    uint32_t time_ms, time_ns;
    p_in >> time_ms >> time_ns;
    float acc_x, acc_y, acc_z;
    p_in >> acc_x >> acc_y >> acc_z;
    float gyr_x, gyr_y, gyr_z;
    p_in >> gyr_x >> gyr_y >> gyr_z;
    
    0xfb, 0xdc, 0xf5, 0xd7, 0x03, 0x00, 0x02, 0x00
    uint32_t time_ms, time_ns;
    p_in >> time_ms >> time_ns;
    uint16_t adc_val;
    int16_t temperature;
    int32_t current;
    uint8_t percent;
    p_in >> adc_val >> temperature >> current >> percent;

*/
int main(int, char**)
{
    auto can_in = can_streambuf("can0", 0x202);
    auto can_cfg = can_streambuf("can0", 0x201);
    std::istream in(&can_in);
    std::iostream cfg(&can_cfg);
    robomaster::package p_in{};
    robomaster::dds::dds dds(cfg);
    dds.send_remove_sub(9, 0, 0);
    dds.send_add_sub();
    cfg.flush();

    int cnt = 0;
    while (true)
    {
        p_in.read_from(in);
        cnt += 1;

        if (p_in.sender != 0x03)
            continue;
        if (p_in.receiver != 0x9)
            continue;
        if (p_in.cmd_set != 0x48)
            continue;
        if (p_in.cmd_id != 0x08)
            continue;

        if (true) //p_in.data.size() == 94)
        {
            p_in.discard(2);
            
            //std::cout << "cnt: " << adc_val << " " << temperature << " " << current << " " << static_cast<int>(percent) << "\n";//, a: " << a << ", b: " << b << "\n";
            //std::cout << ", a: " << std::dec << a << "\n";// ", b: " << b << ", c: " << c << "\n"; //, d: " << d << "\n"; //, d: " << d << "\n";
            if (true) // 190 - 200
            {
                std::cout << p_in << std::dec << cnt << "\n";
                //p_in.write_to(out);
            }
        }

        //std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}

