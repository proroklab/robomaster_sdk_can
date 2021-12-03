#pragma once

#include <vector>
#include <functional>

#include <robomaster_protocol.hpp>

/*
    Unknown uids:
        0xb3, 0xf7, 0xe6, 0x47, 0x03, 0x00, 0x02, 0x00, // esc time (4* uint32) and state (4*uint8)
        0x9c, 0x3d, 0xa1, 0x91, 0x03, 0x00, 0x02, 0x00, // unknown, all zero
        0xa7, 0x02, 0x29, 0x88, 0x03, 0x00, 0x02, 0x00  // turret yaw

    0xb3, 0xf7, 0xe6, 0x47, 0x03, 0x00, 0x02, 0x00
    uint32_t w1_timer_ms, w2_timer_ms, w3_timer_ms, w4_timer_ms;
    p_in >> w1_timer_ms >> w2_timer_ms >> w3_timer_ms >> w4_timer_ms;
    uint8_t w1_state, w2_state, w3_state, w4_state;
    p_in >> w1_state >> w2_state >> w3_state >> w4_state;

*/


namespace robomaster
{

namespace dds
{

struct subject
{
    subject(package& p)
    {
        p >> time_ms >> time_ns;
    }

    uint32_t time_ms, time_ns;
};

struct imu : public subject
{
    imu(package& p)
        : subject(p)
    {
        p >> yaw >> pitch >> roll;
    }

    float yaw, pitch, roll;
};

struct wheel_encoders : public subject
{
    wheel_encoders(package& p)
        : subject(p)
    {
        p >> rpm[0] >> rpm[1] >> rpm[2] >> rpm[3];
        p >> enc[0] >> enc[1] >> enc[2] >> enc[3];
        p >> timer[0] >> timer[1] >> timer[2] >> timer[3];
        p >> state[0] >> state[1] >> state[2] >> state[3];
    }

    int16_t rpm[4];
    uint16_t enc[4];
    uint32_t timer[4];
    uint8_t state[4];
};

struct acc_gyro : public subject
{
    acc_gyro(package& p)
        : subject(p)
    {
        p >> acc_x >> acc_y >> acc_z;
        p >> gyr_x >> gyr_y >> gyr_z;
    }

    float acc_x, acc_y, acc_z;
    float gyr_x, gyr_y, gyr_z;
};

struct battery : public subject
{
    battery(package& p)
        : subject(p)
    {
        p >> adc_val >> temperature >> current >> percent;
    }

    uint16_t adc_val;
    int16_t temperature;
    int32_t current;
    uint8_t percent;
};

struct velocity : public subject
{
    velocity(package& p)
        : subject(p)
    {
        p >> vgx >> vgy >> vgz;
        p >> vbx >> vby >> vbz;
    }
    float vgx, vgy, vgz;
    float vbx, vby, vbz;
};

constexpr static uint8_t BASE_ID = 0x09;
constexpr static uint8_t SUBCONTROLLER_ID = 0x03;
constexpr static uint8_t CMDSET_DDS = 0x48;
constexpr static uint8_t CMDID_DDS_ADD_SUB = 0x03;
constexpr static uint8_t CMDID_DDS_DEL_SUB = 0x04;
constexpr static uint8_t CMDID_DDS_RESET_NODE = 0x02;
constexpr static uint8_t CMDID_DDS_PUSH_MSG = 0x08;

class dds
{
public:
    dds(std::iostream& in, std::iostream& out)
        : _in{in}
        , _out{out}
        , _sender{BASE_ID}
        , _receiver{SUBCONTROLLER_ID}
        , _runner{std::bind(&robomaster::dds::dds::process_incoming, this)}
    {
        _runner.detach();
    }

    void subscribe(std::function<void(const struct imu&)> callback, uint8_t frequency)
    {
        subscribe(callback, frequency, {0x42, 0xee, 0x13, 0x1d, 0x03, 0x00, 0x02, 0x00});
    }

    void subscribe(std::function<void(const struct wheel_encoders&)> callback, uint8_t frequency)
    {
        subscribe(callback, frequency, {0x09, 0xa3, 0x26, 0xe2, 0x03, 0x00, 0x02, 0x00});
    }

    void subscribe(std::function<void(const struct acc_gyro&)> callback, uint8_t frequency)
    {
        subscribe(callback, frequency, {0xf4, 0x1d, 0x1c, 0xdc, 0x03, 0x00, 0x02, 0x00});
    }

    void subscribe(std::function<void(const struct battery&)> callback, uint8_t frequency)
    {
        subscribe(callback, frequency, {0xfb, 0xdc, 0xf5, 0xd7, 0x03, 0x00, 0x02, 0x00});
    }

    void subscribe(std::function<void(const struct velocity&)> callback, uint8_t frequency)
    {
        subscribe(callback, frequency, {0x66, 0x3e, 0x3e, 0x4c, 0x03, 0x00, 0x02, 0x00});
    }

private:
    void process_incoming()
    {
        while (true)
        {
            robomaster::package p{};
            p.read_from(_in);

            if (p.sender == _receiver && p.receiver == _sender && p.cmd_set == CMDSET_DDS && p.cmd_id == CMDID_DDS_PUSH_MSG)
            {
                uint8_t sub_mode;
                uint8_t msg_id;
                p >> sub_mode >> msg_id;

                if (msg_id < _subscriptions.size())
                {
                    _subscriptions[msg_id](p);
                }
            }
        }
    }

    template<class T>
    void subscribe(std::function<void(const T&)> callback, uint8_t frequency, const std::array<uint8_t, 8> subscriber_uid)
    {
        const auto msg_id = _subscriptions.size();
        send_remove_sub(_sender, 0, msg_id);
        send_add_sub({subscriber_uid}, 0, msg_id, frequency);
        _subscriptions.emplace_back([callback](package & p)
        {
            callback(T{p});
        });
    }

    void send_add_sub(const std::vector<std::array<uint8_t, 8>>& subscriber_uids, uint8_t sub_mode, uint8_t msg_id, uint16_t frequency)
    {
        robomaster::package p{_sender, _receiver, CMDSET_DDS, CMDID_DDS_ADD_SUB, false, false};
        p << _sender; // Node ID (this node)
        p << msg_id; // subscription id
        p << static_cast<uint8_t>(0x3); // (self._timestamp & 0x1) | (self._stop_when_disconnect & 0x2)
        p << sub_mode;
        p << static_cast<uint8_t>(subscriber_uids.size()); // number of subscribed types

        for (const auto& subscriber_uid : subscriber_uids)
        {
            p.data.insert(p.data.end(), subscriber_uid.begin(), subscriber_uid.end());
        }

        p << frequency;
        p.write_to(_out);
    }

    void send_remove_sub(uint8_t node_id, uint8_t sub_mode, uint8_t msg_id)
    {
        robomaster::package p{_sender, _receiver, CMDSET_DDS, CMDID_DDS_DEL_SUB, false, false};
        p << sub_mode << node_id  << msg_id;
        p.write_to(_out);
    }

    void send_reset_node(uint8_t node_id)
    {
        robomaster::package p{_sender, _receiver, CMDSET_DDS, CMDID_DDS_RESET_NODE, false, false};
        p << node_id;
        p.write_to(_out);
    }

    std::iostream& _in;
    std::iostream& _out;
    const uint8_t _sender;
    const uint8_t _receiver;

    std::vector<std::function<void(package&)>> _subscriptions;

    std::thread _runner;
};

}
}

