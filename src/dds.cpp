#include <robomaster/dds.hpp>
#include <chrono>
#include <thread>

namespace robomaster
{

    static constexpr uint8_t BASE_ID = 0x09;
    static constexpr uint8_t SUBCONTROLLER_ID = 0x03;
    static constexpr uint8_t CMDSET_DDS = 0x48;
    static constexpr uint8_t CMDID_DDS_ADD_SUB = 0x03;
    static constexpr uint8_t CMDID_DDS_DEL_SUB = 0x04;
    static constexpr uint8_t CMDID_DDS_RESET_NODE = 0x02;
    static constexpr uint8_t CMDID_DDS_PUSH_MSG = 0x08;

    metadata::metadata(package& p)
    {
        p >> time_ms >> time_ns;
    }

    dds::dds(std::iostream& in, std::iostream& out)
        : in_{in}
        , out_{out}
        , sender_{BASE_ID}
        , receiver_{SUBCONTROLLER_ID}
        , should_stop_{false}
        , runner_{&dds::process_incoming, this} {}

    dds::~dds()
    {
        stop();
    }

    void dds::stop()
    {
        should_stop_ = true;

        if (runner_.joinable())
        {
            runner_.join();
        }
    }

    void dds::process_incoming()
    {
        while (!should_stop_)
        {
            try
            {
                package p{};
                p.read_from(in_);

                if (p.sender == receiver_ && p.receiver == sender_ &&
                    p.cmd_set == CMDSET_DDS && p.cmd_id == CMDID_DDS_PUSH_MSG)
                {

                    uint8_t sub_mode;
                    uint8_t msg_id;
                    p >> sub_mode >> msg_id;

                    if (msg_id < subscriptions_.size())
                    {
                        subscriptions_[msg_id](p);
                    }
                }
            }
            catch (const std::exception&)
            {
                // Handle read errors gracefully
                if (!should_stop_)
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            }
        }
    }

    void dds::send_add_sub(const std::vector<std::array<uint8_t, 8>>& subscriber_uids,
        uint8_t sub_mode, uint8_t msg_id, uint16_t frequency)
    {
        package p{sender_, receiver_, CMDSET_DDS, CMDID_DDS_ADD_SUB, false, false};
        p << sender_; // Node ID (this node)
        p << msg_id; // subscription id
        p << static_cast<uint8_t>(0x3); // (self._timestamp & 0x1) | (self._stop_when_disconnect & 0x2)
        p << sub_mode;
        p << static_cast<uint8_t>(subscriber_uids.size()); // number of subscribed types

        for (const auto& subscriber_uid : subscriber_uids)
        {
            p.data.insert(p.data.end(), subscriber_uid.begin(), subscriber_uid.end());
        }

        p << frequency;
        p.write_to(out_);
    }

    void dds::send_remove_sub(uint8_t node_id, uint8_t sub_mode, uint8_t msg_id)
    {
        package p{sender_, receiver_, CMDSET_DDS, CMDID_DDS_DEL_SUB, false, false};
        p << sub_mode << node_id << msg_id;
        p.write_to(out_);
    }

    void dds::send_reset_node(uint8_t node_id)
    {
        package p{sender_, receiver_, CMDSET_DDS, CMDID_DDS_RESET_NODE, false, false};
        p << node_id;
        p.write_to(out_);
    }

} // namespace robomaster
