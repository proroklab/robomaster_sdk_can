#include <vector>
#include <string>
#include <exception>

class CANManager {
public:
    CANManager(const std::string iface_name, const unsigned int can_id)
    {
        if ((_pf.fd = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
            throw std::system_error(EFAULT, std::generic_category());
        }
        _pf.events = POLLIN;

        struct can_filter rfilter;
        rfilter.can_id = can_id;
        rfilter.can_mask = CAN_EFF_MASK;
        setsockopt(_pf.fd, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));

        struct ifreq ifr {};
        strncpy(ifr.ifr_name, iface_name.c_str(), iface_name.size());

        ifr.ifr_ifindex = if_nametoindex(ifr.ifr_name);
        if (!ifr.ifr_ifindex) {
            throw std::system_error(EFAULT, std::generic_category());
        }

        _addr.can_family = AF_CAN;
        _addr.can_ifindex = ifr.ifr_ifindex;

        if (bind(_pf.fd, (struct sockaddr *)&_addr, sizeof(_addr)) < 0) {
            throw std::system_error(EFAULT, std::generic_category());
        }
    }

    ~CANManager() {
        close(_pf.fd);
    }

    int Run() {
        struct can_frame frame {};

        frame.can_id = 0xab;
        frame.can_dlc = 8;

        if (write(_pf.fd, &frame, CAN_MTU) != CAN_MTU) {
            perror("write");
            return 1;
        }

        int ready;
        ready = poll(&_pf, 1, 10);
        if (ready == -1)
            return 1;

        struct msghdr msg;
        struct iovec iov;
        struct can_frame rx_frame;
        iov.iov_base = &rx_frame;
        msg.msg_name = &_addr;
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;

        for (int i = 0; i < ready; i++) {
            iov.iov_len = sizeof(rx_frame);
            msg.msg_namelen = sizeof(_addr);
            msg.msg_controllen = 0;
            msg.msg_flags = 0;

            int nbytes = recvmsg(_pf.fd, &msg, 0);
            if (nbytes < 0) {
                perror("read");
                return 1;
            }

            if ((size_t)nbytes != CAN_MTU) {
                fprintf(stderr, "read: nbytes\n");
                return 1;
            }
            printf("frame id %d\n", rx_frame.can_id);
        }

        return 0;
    }
private:
    struct sockaddr_can _addr {};
    struct pollfd _pf;
};
