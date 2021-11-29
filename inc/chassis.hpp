#include <vector>

#include <robomaster_protocol.hpp>

namespace robomaster
{
namespace command
{
class chassis
{
public:
    chassis(std::iostream& io)
        : _io{io}
    {}

    void send_heartbeat()
    {
        robomaster::package p_heartbeat
        {
            0x09, 0x03, 0x3f, 0x60, false, false,
            {
                0x00, 0x04, 0x20, 0x00, 0x01, 0x00, 0x40, 0x00,
                0x02, 0x10, 0x04, 0x03, 0x00, 0x04
            }
        };
        p_heartbeat.write_to(_io);
    }

    void send_wheel_speed(int16_t w1, int16_t w2, int16_t w3, int16_t w4)
    {
        // w1 fr, w2 fl, w3 bl, w4 br
        robomaster::package pkg{0xC9, 0xC3, 0x3f, 0x20, false, false};
        pkg << w1 << w2 << w3 << w4;
        pkg.write_to(_io);
    }

    void send_speed(float vx, float vy, float omega)
    {
        robomaster::package pkg{0xC9, 0xC3, 0x3f, 0x21, false, false};
        pkg << vx << vy << omega;
        pkg.write_to(_io);
    }

private:
    std::iostream& _io;
};

}
}

