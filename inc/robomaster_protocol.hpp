#include <vector>
#include <string>
#include <exception>
#include <cstring>
#include <streambuf>
#include <thread>
#include <map>

#include <crc.hpp>

namespace robomaster {

class package {
public:
    package() : _id{0}, _count{0}, _data{} {}
    package(unsigned int id, std::vector<uint8_t>& data) : _id{id}, _count{0}, _data{data} {}

    constexpr static uint8_t MAGIC_NUMBER_A = 0x55;
    constexpr static uint8_t MAGIC_NUMBER_B = 0x04;
    constexpr static uint8_t CRC_HEADER_INIT = 119;
    constexpr static uint16_t CRC_PACKAGE_INIT = 13970;

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

    friend std::ostream & operator << (std::ostream &out, const package &c);
    friend std::istream & operator >> (std::istream &in,  package &c);

private:

    unsigned int _id;
    unsigned int _count;
    std::vector<uint8_t> _data;
};

class id_tracker {
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
    std::vector<uint8_t> serial_data = {
        package::MAGIC_NUMBER_A,
        static_cast<uint8_t>(pkg._data.size() + 10),
        package::MAGIC_NUMBER_B
    };
    const auto crc_header = crc::compute_crc8(package::CRC_HEADER_INIT, serial_data);
    serial_data.push_back(crc_header);

    serial_data.insert(serial_data.end(), {
        static_cast<uint8_t>(pkg._id & 0xff),
        static_cast<uint8_t>((pkg._id & 0xff00) >> 8),
        static_cast<uint8_t>(id_count & 0xff),
        static_cast<uint8_t>((id_count & 0xff00) >> 8),
    });

    serial_data.insert(serial_data.end(), pkg._data.begin(), pkg._data.end());

    const auto crc_package = crc::compute_crc16(package::CRC_PACKAGE_INIT, serial_data);
    serial_data.insert(serial_data.end(), {
        static_cast<uint8_t>(crc_package & 0xff),
        static_cast<uint8_t>((crc_package & 0xff00) >> 8),
    });
    return out.write(reinterpret_cast<char*>(serial_data.data()), serial_data.size());
}

std::istream& operator>>(std::istream& in, package& pkg)
{
    while(true)
    {
        uint8_t magic_number_a = 0;
        while(magic_number_a != package::MAGIC_NUMBER_A) {
            in.read(reinterpret_cast<char*>(&magic_number_a), 1);
        }

        uint8_t length_parsed;
        in.read(reinterpret_cast<char*>(&length_parsed), 1);

        uint8_t magic_number_b;
        in.read(reinterpret_cast<char*>(&magic_number_b), 1);
        if (magic_number_b != package::MAGIC_NUMBER_B) {
            continue;
        }

        uint8_t crc_header_parsed;
        in.read(reinterpret_cast<char*>(&crc_header_parsed), 1);

        std::vector<uint8_t> data{
            package::MAGIC_NUMBER_A,
            length_parsed,
            package::MAGIC_NUMBER_B
        };
        const auto crc_header_expected = crc::compute_crc8(package::CRC_HEADER_INIT, data);

        if (crc_header_parsed != crc_header_expected) {
            continue;
        }
        data.push_back(crc_header_parsed);
        data.resize(length_parsed - 2); // don't consider checksum
        in.read(reinterpret_cast<char*>(&data[4]), length_parsed - 4 - 2); // don't parse header and checksum

        uint16_t crc_package_parsed;
        in.read(reinterpret_cast<char*>(&crc_package_parsed), 2);
        // Finished parsing, verify package checksum

        const auto crc_package_expected = crc::compute_crc16(package::CRC_PACKAGE_INIT, data);
        if (crc_package_parsed != crc_package_expected) {
            continue;
        }

        // now move into package data
        pkg._id = data[4] | ((static_cast<uint16_t>(data[5]) << 8) & 0xff00);
        pkg._count = data[6] | ((static_cast<uint16_t>(data[7]) << 8) & 0xff00);
        pkg._data.assign(data.begin() + 8, data.end());

        return in;
    }
}
}

