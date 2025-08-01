#pragma once

#include <cstdint>
#include <vector>

namespace robomaster
{

uint8_t crc8(uint8_t seed, const std::vector<uint8_t>& data);
uint16_t crc16(uint16_t seed, const std::vector<uint8_t>& data);

} // namespace robomaster
