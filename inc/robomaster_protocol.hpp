#include <vector>
#include <string>
#include <exception>
#include <cstring>
#include <streambuf>
#include <thread>
#include <deque>
#include <map>

#include <crc.hpp>

namespace robomaster
{

constexpr static uint8_t MAGIC_NUMBER_A = 0x55;
constexpr static uint8_t MAGIC_NUMBER_B = 0x04;
constexpr static uint8_t CRC_HEADER_INIT = 119;
constexpr static uint16_t CRC_PACKAGE_INIT = 13970;

class package
{
public:
    package() : _id{0}, _count{0}, _data{} {}
    package(uint16_t id) : _id{id}, _count{0}, _data{} {}
    package(uint16_t id, std::deque<uint8_t>& data) : _id{id}, _count{0}, _data{data} {}

    auto get_id() const
    {
        return _id;
    }

    auto get_count() const
    {
        return _count;
    }

    auto& get_data() const
    {
        return _data;
    }

    template <typename T>
    auto put(const T var)
    {
        auto bytes = reinterpret_cast<const uint8_t*>(&var);
        _data.insert(_data.end(), bytes, bytes + sizeof(T));
        return this;
    }

    template <typename T>
    auto get(T& var)
    {
        std::vector<uint8_t> data{_data.begin(), _data.begin() + sizeof(T)};
        var = *reinterpret_cast<T*>(data.data());
        _data.erase(_data.begin(), _data.begin() + sizeof(T));
        return this;
    }

private:
    friend std::ostream& operator << (std::ostream& out, const package& c);
    friend std::istream& operator >> (std::istream& in,  package& c);

    uint16_t _id;
    uint16_t _count;
    std::deque<uint8_t> _data;
};

template <typename T>
package& operator<<(package& pkg, const T& var)
{
    return *pkg.put(var);
}

template <typename T>
package& operator>>(package& pkg, T& var)
{
    return *pkg.get(var);
}

class id_tracker
{
public:
    id_tracker() {}

    unsigned int get_count_for_id(unsigned int id)
    {
        const auto it = _id_counts.find(id);

        if (it == _id_counts.end())
        {
            _id_counts[id] = 0;
            return 0;
        }
        else
        {
            auto count = it->second;
            it->second += 1;
            return count;
        }
    }

    static id_tracker& get_instance()
    {
        static id_tracker instance;
        return instance;
    }

private:
    std::map<unsigned int, unsigned int> _id_counts;

};

std::ostream& operator<<(std::ostream& out, const package& pkg)
{
    const auto id_count = id_tracker::get_instance().get_count_for_id(pkg._id);
    std::vector<uint8_t> serial_data =
    {
        MAGIC_NUMBER_A,
        static_cast<uint8_t>(pkg._data.size() + 10),
        MAGIC_NUMBER_B
    };
    const auto crc_header = crc::crc(CRC_HEADER_INIT, serial_data);
    serial_data.push_back(crc_header);

    auto pkg_id_b = reinterpret_cast<const uint8_t*>(&pkg._id);
    serial_data.insert(serial_data.end(), pkg_id_b, pkg_id_b + 2);
    auto id_count_b = reinterpret_cast<const uint8_t*>(&id_count);
    serial_data.insert(serial_data.end(), id_count_b, id_count_b + 2);

    serial_data.insert(serial_data.end(), pkg._data.begin(), pkg._data.end());

    const auto crc_package = crc::crc(CRC_PACKAGE_INIT, serial_data);
    auto crc_package_b = reinterpret_cast<const uint8_t*>(&crc_package);
    serial_data.insert(serial_data.end(), crc_package_b, crc_package_b + 2);

    return out.write(reinterpret_cast<char*>(serial_data.data()), serial_data.size());
}

std::istream& operator>>(std::istream& in, package& pkg)
{
    while (true)
    {
        while (in.get() != MAGIC_NUMBER_A);

        uint8_t length_parsed = in.get();
        uint8_t magic_number_b = in.get();

        if (magic_number_b != MAGIC_NUMBER_B)
        {
            continue;
        }

        uint8_t crc_header_parsed = in.get();
        std::vector<uint8_t> data{MAGIC_NUMBER_A, length_parsed, MAGIC_NUMBER_B};
        const auto crc_header_expected = crc::crc(CRC_HEADER_INIT, data);

        if (crc_header_parsed != crc_header_expected)
        {
            continue;
        }

        data.push_back(crc_header_parsed);
        data.resize(length_parsed - 2); // don't consider checksum
        in.read(reinterpret_cast<char*>(&data[4]), length_parsed - 4 - 2); // don't parse header and checksum

        uint16_t crc_package_parsed;
        in.read(reinterpret_cast<char*>(&crc_package_parsed), 2);

        // Finished parsing, verify package checksum
        const auto crc_package_expected = crc::crc(CRC_PACKAGE_INIT, data);

        if (crc_package_parsed != crc_package_expected)
        {
            continue;
        }

        // now move into package data
        pkg._id = *(reinterpret_cast<uint16_t*>(&data[4]));
        pkg._count = *(reinterpret_cast<uint16_t*>(&data[6]));
        pkg._data.assign(data.begin() + 8, data.end());

        return in;
    }
}
}

