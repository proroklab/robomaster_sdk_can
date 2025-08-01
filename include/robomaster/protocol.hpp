#pragma once

#include <vector>
#include <exception>
#include <deque>
#include <iostream>
#include <cstdint>
#include <map>
#include <string>

namespace robomaster
{

class protocol_error : public std::exception
{
public:
    explicit protocol_error(const std::string& message);
    const char* what() const noexcept override;

private:
    std::string message_;
};

class id_tracker
{
public:
    unsigned int get_count_for_id(uint16_t id);
    static id_tracker& get_instance();

private:
    id_tracker() = default;
    std::map<uint16_t, unsigned int> seq_id_counts_;
};

class package
{
public:
    package();
    package(uint8_t sender, uint8_t receiver, uint8_t cmd_set, uint8_t cmd_id, bool is_ack, bool need_ack);
    package(uint8_t sender, uint8_t receiver, uint8_t cmd_set, uint8_t cmd_id, bool is_ack, bool need_ack, const std::deque<uint8_t>& data);

    template <typename T>
    void put(const T var);

    void discard(size_t n);

    template <typename T>
    void get(T& var);

    void write_to(std::ostream& out);
    void read_from(std::istream& in);

    bool is_ack;
    bool need_ack;
    uint16_t seq_id;
    uint8_t sender;
    uint8_t receiver;
    uint8_t cmd_set;
    uint8_t cmd_id;

    std::deque<uint8_t> data;
};

template <typename T>
package& operator<<(package& pkg, const T& var);

template <typename T>
package& operator>>(package& pkg, T& var);

std::ostream& operator<<(std::ostream& out, package& p);

// Template implementations
template <typename T>
void package::put(const T var)
{
    auto bytes = reinterpret_cast<const uint8_t*>(&var);
    data.insert(data.end(), bytes, bytes + sizeof(T));
}

template <typename T>
void package::get(T& var)
{
    if (data.size() < sizeof(T))
    {
        throw protocol_error("Insufficient data in package");
    }

    std::vector<uint8_t> d{data.begin(), data.begin() + sizeof(T)};
    var = *reinterpret_cast<T*>(d.data());
    discard(sizeof(T));
}

template <typename T>
package& operator<<(package& pkg, const T& var)
{
    pkg.put(var);
    return pkg;
}

template <typename T>
package& operator>>(package& pkg, T& var)
{
    pkg.get(var);
    return pkg;
}

} // namespace robomaster
