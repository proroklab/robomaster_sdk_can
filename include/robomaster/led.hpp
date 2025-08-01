#pragma once

#include <robomaster/protocol.hpp>
#include <iostream>

namespace robomaster
{

class led
{
public:
    explicit led(std::iostream& io);

    void send_led(uint8_t mode, uint8_t r, uint8_t g, uint8_t b,
        uint16_t speed_up, uint16_t speed_down, uint8_t led_mask);

private:
    std::iostream& io_;
};

} // namespace robomaster
