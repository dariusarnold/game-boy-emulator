#include "bitmanipulation.hpp"
#include <cassert>

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
    constexpr int WHOLE_BYTE = 0xFF;
    return (x >> constants::BYTE_SIZE) & WHOLE_BYTE;
}
uint8_t get_low_byte(uint16_t x) {
    constexpr int WHOLE_BYTE = 0xFF;
    return x & WHOLE_BYTE;
}

uint8_t get_low_nibble(uint8_t x) {
    constexpr int LOW_NIBBLE = 0x0F;
    return x & LOW_NIBBLE;
}
uint8_t get_high_nibble(uint8_t x) {
    constexpr int HIGH_NIBBLE = 0xF0;
    return (x & HIGH_NIBBLE) >> (constants::BYTE_SIZE / 2);
}

uint8_t rotate_left(uint8_t x) {
    return (x << 1);
}

uint8_t rotate_left_carry(uint8_t x, bool& carry_flag) {
    bool prev_carry_flag = carry_flag;
    constexpr int HIGHEST_BIT_IN_BYTE = 7;
    carry_flag = is_bit_set(x, HIGHEST_BIT_IN_BYTE);
    x = x << 1;
    set(x, 0, prev_carry_flag ? BitValues::Active : BitValues::Inactive);
    return x;
}

uint16_t word_from_bytes(uint8_t high_byte, uint8_t low_byte) {
    return (high_byte << constants::BYTE_SIZE) + low_byte;
}
} // namespace bitmanip
