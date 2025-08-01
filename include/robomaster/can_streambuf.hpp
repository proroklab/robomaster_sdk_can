#pragma once

#include <string>
#include <exception>
#include <streambuf>
#include <system_error>

#include <poll.h>
#include <linux/can.h>

namespace robomaster
{

class can_error : public std::system_error
{
public:
    explicit can_error(const std::string& message);
    explicit can_error(int error_code, const std::string& message);
};

class can_streambuf : public std::basic_streambuf<char>
{
public:
    explicit can_streambuf(const std::string& iface_name, unsigned int can_id);
    ~can_streambuf();

    // Non-copyable, non-movable for simplicity
    can_streambuf(const can_streambuf&) = delete;
    can_streambuf& operator=(const can_streambuf&) = delete;

private:
    int sync() override;
    int_type overflow(int_type ch) override;
    int_type underflow() override;

    struct sockaddr_can _addr {};

    struct pollfd _pf;
    char inbuf[8];
    char outbuf[8];
    canid_t _can_id;
};

} // namespace robomaster
