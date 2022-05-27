#pragma once

#include "constants.h"
#include <cstdint>

namespace bitmanip {
     /**
      * Set bit at position to value
      * @param input Byte to modify
      * @param position 0..7 where 0 is the least significant bit
      * @param value 0..1
      */
    void set(uint8_t& input, uint8_t position, BitValues value);
    // Set bit in input at position to 1
    void set(uint8_t& input, uint8_t position);
    // Set bit in input at position to 0
    void unset(uint8_t& input, uint8_t position);

    template <typename T>
    bool is_bit_set(T input, int position) {
        static_assert(std::is_integral_v<T>);
        return (input & (1 << position)) > 0;
    }

    // Get most significant byte
    uint8_t get_high_byte(uint16_t x);
    // Get least significant byte
    uint8_t get_low_byte(uint16_t x);

    // Examle 0x34 -> 0x04
    uint8_t get_low_nibble(uint8_t);
    // Exaple 0x34 -> 0x03
    uint8_t get_high_nibble(uint8_t);

    uint8_t rotate_left(uint8_t x);

    uint8_t rotate_left_carry(uint8_t x, bool& carry_flag);

    uint16_t word_from_bytes(uint8_t high_byte, uint8_t low_byte);

} // namespace bitmanip