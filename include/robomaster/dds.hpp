#pragma once

#include <vector>
#include <functional>
#include <thread>
#include <atomic>
#include <iostream>
#include <robomaster/protocol.hpp>

namespace robomaster
{

struct metadata
{
    metadata(package& p);
    uint32_t time_ms, time_ns;
};

template <typename T>
constexpr std::array<uint8_t, 8> get_uid();

class dds
{
public:
    dds(std::iostream& in, std::iostream& out);
    ~dds();

    // Non-copyable, non-movable
    dds(const dds&) = delete;
    dds& operator=(const dds&) = delete;

    template<class ...args>
    void subscribe(std::function<void(const metadata&, const args& ...)> callback, uint8_t frequency);

    void stop();

private:
    void process_incoming();
    void send_add_sub(const std::vector<std::array<uint8_t, 8>>& subscriber_uids, uint8_t sub_mode, uint8_t msg_id, uint16_t frequency);
    void send_remove_sub(uint8_t node_id, uint8_t sub_mode, uint8_t msg_id);
    void send_reset_node(uint8_t node_id);

    std::iostream& in_;
    std::iostream& out_;
    const uint8_t sender_;
    const uint8_t receiver_;

    std::vector<std::function<void(package&)>> subscriptions_;
    std::atomic<bool> should_stop_;
    std::thread runner_;
};

// Template implementation
template<class ...args>
void dds::subscribe(std::function<void(const metadata&, const args& ...)> callback, uint8_t frequency)
{
    const auto msg_id = subscriptions_.size();
    send_remove_sub(sender_, 0, msg_id);
    send_add_sub({get_uid<args>()...}, 0, msg_id, frequency);
    subscriptions_.emplace_back([callback](package& p)
    {
        callback(metadata{p}, args{p}...);
    });
}

} // namespace robomaster
