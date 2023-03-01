#include "bitmanipulation.hpp"
#include <cassert>

namespace bitmanip {

void set(uint8_t& input, uint8_t position) {
    set_bit(input, position, 1);
}

void unset(uint8_t& input, uint8_t position) {
    set_bit(input, position, 0);
}

void set_bit(uint8_t& input, uint8_t position, uint8_t value) {
    assert(position < 8 && "A byte only has 8 bits");
    input = (input & ~(1U << position)) | ((value & 1U) << position);
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

uint8_t swap_nibbles(uint8_t x) {
    return (get_low_nibble(x) << 4) | get_high_nibble(x);
}

uint8_t rotate_left(uint8_t x) {
    return static_cast<uint8_t>(x << 1);
}

uint8_t rotate_left_carry(uint8_t x, bool& carry_flag) {
    bool prev_carry_flag = carry_flag;
    constexpr int HIGHEST_BIT_IN_BYTE = 7;
    constexpr int LOWEST_BIT_IN_BYTE = 0;
    carry_flag = is_bit_set(x, HIGHEST_BIT_IN_BYTE);
    x = x << 1;
    set_bit(x, LOWEST_BIT_IN_BYTE, static_cast<uint8_t>(prev_carry_flag));
    return x;
}

uint8_t rotate_right(uint8_t x) {
    return x >> 1;
}

uint8_t rotate_right_carry(uint8_t x, bool& carry_flag) {
    bool prev_carry_flag = carry_flag;
    constexpr int HIGHEST_BIT_IN_BYTE = 7;
    constexpr int LOWEST_BIT_IN_BYTE = 0;
    carry_flag = is_bit_set(x, LOWEST_BIT_IN_BYTE);
    x = x >> 1;
    set_bit(x, HIGHEST_BIT_IN_BYTE, static_cast<uint8_t>(prev_carry_flag));
    return x;
}

} // namespace bitmanip
