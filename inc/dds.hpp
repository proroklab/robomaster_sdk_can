#pragma once

#include <vector>

#include <robomaster_protocol.hpp>

namespace robomaster
{
namespace dds
{
class dds
{
public:
    dds(std::iostream& io)
        : _io{io}
    {}

    //void subscribe(
    void send_add_sub()
    {
        robomaster::package pkg{0x09, 0x03, 0x48, 0x03, false, false, {
                0x09, // Node id
                0x00, // msg id (user definable)
                0x03, // (self._timestamp & 0x1) | (self._stop_when_disconnect & 0x2)
                0x00, // sub node (0)
                0x01, // sub data num
                
                //0xb3, 0xf7, 0xe6, 0x47, 0x03, 0x00, 0x02, 0x00, // esc time (4* uint32) and state (4*uint8)
                //0x42, 0xee, 0x13, 0x1d, 0x03, 0x00, 0x02, 0x00, // IMU? time, time, yaw, pitch, toll
                //0x09, 0xa3, 0x26, 0xe2, 0x03, 0x00, 0x02, 0x00, // Encoder data
                //0xf4, 0x1d, 0x1c, 0xdc, 0x03, 0x00, 0x02, 0x00, // ACC and GYR
                //0x9c, 0x3d, 0xa1, 0x91, 0x03, 0x00, 0x02, 0x00, // unknown
                0xfb, 0xdc, 0xf5, 0xd7, 0x03, 0x00, 0x02, 0x00, // battery
                
                0x14, 0x00 // freq (20 Hz)
            }
        };
        
        /*
09>03 [4803]   l=  23 d=09 01 03 00 02
                    a7 02 29 88 03 00 02 00
                    66 3e 3e 4c 03 00 02 00
                    32 00 116

        */
        
        

        pkg.write_to(_io);
    }

    void send_remove_sub(uint8_t node_id, uint8_t sub_mode, uint8_t msg_id)
    {
        robomaster::package p{0x09, 0x03, 0x48, 0x04, false, false};
        p << sub_mode << node_id  << msg_id;
        p.write_to(_io);
    }

private:
    std::iostream& _io;
};

}
}

