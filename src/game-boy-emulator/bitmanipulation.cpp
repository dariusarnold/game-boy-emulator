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


    } // namespace bitmanip