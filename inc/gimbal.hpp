#pragma once

#include <protocol.hpp>
#include <dds.hpp>
namespace robomaster
{
namespace command
{
class gimbal
{
public:
    gimbal(std::iostream& io)
        : _io{io}
    {}

    

    void send_workmode(uint8_t mode)
    {
        robomaster::package pkg{0x09, 0x04, 0x4, 0x4c, false, false};
        pkg << mode << 0x00;
        pkg.write_to(_io);
    }

    void recenter()
    {
        robomaster::package pkg{0xC9, 0x04, 0x3f, 0xb2, false, false,
            {
            0x00, 0x08, 0x05, 0x64, 0x00, 0x00, 0x00, 0x64, 0x00
            }
        };
        
        pkg.write_to(_io);
    }

    void send_angles(int16_t yaw, int16_t pitch)
    {
        // The rotations applied to the gimbal are the input angles divided by 10. (so send 10 times the desired angle)
        uint8_t low_yaw = yaw & 0xFF;
        uint8_t high_yaw = (yaw >> 8) & 0xFF;
        uint8_t low_pitch = pitch & 0xFF;
        uint8_t high_pitch = (pitch >> 8) & 0xFF;
        robomaster::package pkg{0xC9, 0x04, 0x3f, 0xb0, false, false,
            {
            0x00, 0x08, 0x1d, low_yaw ,high_yaw, 0x00, 0x00, low_pitch, high_pitch, 
            0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x1e, 0x00
            }
        };
        
        pkg.write_to(_io);
    }

private:
    std::iostream& _io;
};

}

}