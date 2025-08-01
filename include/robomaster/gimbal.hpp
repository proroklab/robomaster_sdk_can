#pragma once

#include <robomaster/protocol.hpp>
#include <iostream>

namespace robomaster
{

class gimbal
{
public:
    gimbal(std::iostream& io);

    void send_workmode(uint8_t mode);
    void recenter();
    void send_angles(int16_t yaw, int16_t pitch);

private:
    std::iostream& io_;
};

} // namespace robomaster
