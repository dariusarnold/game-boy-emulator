#include "bitmanipulation.hpp"

namespace bitmanip {

    void set(uint8_t& input, uint8_t position) {
        set(input, position, BitValues::Active);
    }

    void unset(uint8_t& input, uint8_t position) {
        set(input, position, BitValues::Inactive);
    }

    void set(uint8_t& input, uint8_t position, BitValues value) {
        input ^= (-as_integral(value) ^ input) & (1U << position);
    }
    uint8_t get_high_byte(uint16_t x) {
        return (x >> constants::BYTE_SIZE) & 0XFF;
    }
    uint8_t get_low_byte(uint16_t x) {
        return x & 0xFF;
    }
} // namespace bitmanip