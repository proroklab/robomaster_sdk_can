#pragma once

#include <protocol.cpp>

namespace robomaster
{
namespace command
{
class led
{
public:
    led(std::iostream& io)
        : _io{io}
    {}

    void send_led(uint8_t mode, uint8_t r, uint8_t g, uint8_t b, uint16_t speed_up, uint16_t speed_down, uint8_t led_mask)
    {
        robomaster::package pkg{0x09, 0x18, 0x3f, 0x32, false, false};

        pkg << mode;
        pkg << static_cast<uint8_t>(0xff);
        pkg << static_cast<uint8_t>(0);
        pkg << r << g << b;
        pkg << static_cast<uint8_t>(0);
        pkg << speed_up << speed_down;
        pkg << led_mask;
        pkg << static_cast<uint8_t>(0);

        pkg.write_to(_io);
    }

private:
    std::iostream& _io;
};

}
}

