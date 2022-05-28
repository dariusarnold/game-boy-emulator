#include "bitmanipulation.hpp"

namespace bitmanip {

    // TODO This overload is not clearly distinguished by name from set
    // TODO position as enum
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

    uint8_t get_low_nibble(uint8_t x) {
        return x & 0x0F;
    }
    uint8_t get_high_nibble(uint8_t x) {
        return (x & 0xF0) >> (constants::BYTE_SIZE / 2);
    }

    uint8_t rotate_left(uint8_t x) {
        return (x << 1);
    }

    uint8_t rotate_left_carry(uint8_t x, bool& carry_flag) {
        bool prev_carry_flag = carry_flag;
        carry_flag = is_bit_set(x, 7);
        x = x << 1;
        set(x, 0, prev_carry_flag ? BitValues::Active : BitValues::Inactive);
        return x;
    }

    uint16_t word_from_bytes(uint8_t high_byte, uint8_t low_byte) {
        return (high_byte << constants::BYTE_SIZE) + low_byte;
    }
    std::array<uint8_t, 8> convert_tiles(uint8_t byte1, uint8_t byte2)  {
        std::array<uint8_t, 8> pixels = {0};
        for (int i = 7; i >= 0; --i) {
            auto msb = bitmanip::bit_value(byte2, i);
            auto lsb = bitmanip::bit_value(byte1, i);
            pixels[7 - i] = (msb << 1) + lsb;
        }
        return pixels;
    }


    } // namespace bitmanip