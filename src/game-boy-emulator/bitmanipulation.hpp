#pragma once

#include "constants.h"
#include <cstdint>
#include <span>

namespace bitmanip {
// Set bit in input at position to 1
void set(uint8_t& input, uint8_t position);
// Set bit in input at position to 0
void unset(uint8_t& input, uint8_t position);

/**
 * Set bit at position to value
 * @param input Byte to modify
 * @param position 0..7 where 0 is the least significant bit
 */;
void set_bit(uint8_t& input, uint8_t position, uint8_t value);

template <typename T, typename U>
bool is_bit_set(T input, U position) {
    static_assert(std::is_integral_v<T>);
    return (input & (1 << position)) > 0;
}

template <typename T>
uint8_t bit_value(T input, int position) {
    static_assert(std::is_integral_v<T>);
    return (input >> position) & 1;
}

// Get most significant byte
uint8_t get_high_byte(uint16_t x);
// Get least significant byte
uint8_t get_low_byte(uint16_t x);

// Example 0x34 -> 0x04
uint8_t get_low_nibble(uint8_t x);
// Exampple 0x34 -> 0x03
uint8_t get_high_nibble(uint8_t x);

uint8_t rotate_left(uint8_t x);

uint8_t rotate_left_carry(uint8_t x, bool& carry_flag);

uint8_t rotate_right(uint8_t x);

uint8_t rotate_right_carry(uint8_t x, bool& carry_flag);

uint8_t swap_nibbles(uint8_t x);

uint16_t word_from_bytes(uint8_t high_byte, uint8_t low_byte);

size_t mask(size_t value, uint8_t size_mask);

} // namespace bitmanip
