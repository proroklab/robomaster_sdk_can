#include <robomaster/led.hpp>

namespace robomaster
{

    led::led(std::iostream& io) : io_{io} {}

    void led::send_led(uint8_t mode, uint8_t r, uint8_t g, uint8_t b,
        uint16_t speed_up, uint16_t speed_down, uint8_t led_mask)
    {
        package pkg{0x09, 0x18, 0x3f, 0x32, false, false};

        pkg << mode;
        pkg << static_cast<uint8_t>(0xff);
        pkg << static_cast<uint8_t>(0);
        pkg << r << g << b;
        pkg << static_cast<uint8_t>(0);
        pkg << speed_up << speed_down;
        pkg << led_mask;
        pkg << static_cast<uint8_t>(0);

        pkg.write_to(io_);
    }

} // namespace robomaster
