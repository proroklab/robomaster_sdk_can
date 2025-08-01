#include <robomaster/can_streambuf.hpp>

#include <cstring>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>

namespace robomaster
{

can_error::can_error(const std::string& message)
    : std::system_error(errno, std::generic_category(), message) {}

can_error::can_error(int error_code, const std::string& message)
    : std::system_error(error_code, std::generic_category(), message) {}

can_streambuf::can_streambuf(const std::string& iface_name, unsigned int can_id)
    : _can_id{can_id}
{

    if ((_pf.fd = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0)
    {
        throw can_error("Failed to create CAN socket");
    }

    _pf.events = POLLIN;

    struct can_filter rfilter;
    rfilter.can_id = _can_id;
    rfilter.can_mask = CAN_EFF_MASK;

    if (setsockopt(_pf.fd, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter)) < 0)
    {
        close(_pf.fd);
        throw can_error("Failed to set CAN filter");
    }

    struct ifreq ifr {};

    strncpy(ifr.ifr_name, iface_name.c_str(), std::min(iface_name.size(), sizeof(ifr.ifr_name) - 1));

    ifr.ifr_ifindex = if_nametoindex(ifr.ifr_name);

    if (!ifr.ifr_ifindex)
    {
        close(_pf.fd);
        throw can_error("Invalid CAN interface: " + iface_name);
    }

    _addr.can_family = AF_CAN;
    _addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(_pf.fd, (struct sockaddr*)& _addr, sizeof(_addr)) < 0)
    {
        close(_pf.fd);
        throw can_error("Failed to bind CAN socket");
    }

    // Initialize stream buffers
    setg(inbuf, inbuf, inbuf);
    setp(outbuf, outbuf + 7);
}

can_streambuf::~can_streambuf()
{
    if (_pf.fd >= 0)
    {
        close(_pf.fd);
    }
}

int can_streambuf::sync()
{
    struct can_frame frame;
    frame.can_id = _can_id;
    frame.can_dlc = pptr() - pbase();

    if (frame.can_dlc > 0)
    {
        std::memmove(frame.data, outbuf, frame.can_dlc);

        if (write(_pf.fd, &frame, sizeof(frame)) != sizeof(frame))
        {
            return -1;
        }

        pbump(pbase() - pptr());
    }

    return 0;
}

can_streambuf::int_type can_streambuf::overflow(int_type ch)
{
    *pptr() = ch;
    pbump(1);

    return (sync() == -1 ? std::char_traits<char>::eof() : ch);
}

can_streambuf::int_type can_streambuf::underflow()
{
    struct can_frame rx_frame;
    const auto nbytes = read(_pf.fd, &rx_frame, sizeof(struct can_frame));

    if (nbytes <= 0)
    {
        return std::char_traits<char>::eof();
    }

    std::memmove(inbuf, rx_frame.data, rx_frame.can_dlc);

    setg(inbuf, inbuf, inbuf + rx_frame.can_dlc);
    return std::char_traits<char>::to_int_type(* gptr());
}

} // namespace robomaster
