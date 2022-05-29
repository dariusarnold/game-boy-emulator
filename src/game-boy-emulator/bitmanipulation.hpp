#pragma once

#include "constants.h"
#include <cstdint>
#include <span>
#include "fmt/format.h"

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

    template <typename T>
    uint8_t bit_value(T input, int position) {
        static_assert(std::is_integral_v<T>);
        return (input >> position) & 1;
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

    std::array<uint8_t, 8> convert_tiles(uint8_t byte1, uint8_t byte2);
    std::array<uint32_t, 64> tile_to_gb_color(std::span<uint8_t, 16> tile_data);

    template <typename It>
    void map_gb_color_to_rgba(It begin, It end) {
        constexpr uint32_t palette[4] = {0xff0fbc9b, 0xff0fac8b, 0xff306230, 0xff0f380f,};
        std::for_each(begin, end, [palette](auto& x) { x = palette[x]; });
    }

    } // namespace bitmanip