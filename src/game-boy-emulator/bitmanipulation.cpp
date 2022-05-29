#include <span>
#include <fmt/core.h>
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

    /*
     * Convert two bytes of a tile (which represent a row of 8 pixels) to the 4 color values
     * available on the game boy.
     * Will return an array of values 0b0, 0b01, 0b10, 0b11, which has to be colorized by a palette.
     */
    std::array<uint8_t, 8> convert_tiles(uint8_t byte1, uint8_t byte2)  {
        std::array<uint8_t, 8> pixels = {0};
        for (int i = 7; i >= 0; --i) {
            auto msb = bitmanip::bit_value(byte2, i);
            auto lsb = bitmanip::bit_value(byte1, i);
            pixels[7 - i] = (msb << 1) + lsb;
        }
        return pixels;
    }

    /**
    * VRAM has space for 384 tiles, where each tile is 16 bytes.
    * This takes one tile and converts it from the 2bpp format to rgba format
     */
    std::array<uint32_t, 64> tile_to_gb_color(std::span<uint8_t, 16> tile_data) {
        std::array<uint32_t, 64> out;
        for (int i = 0; i < 16; i += 2) {
            auto row = bitmanip::convert_tiles(tile_data[i], tile_data[i + 1]);
            for (int j = 0; j < row.size(); j++) {
                auto index = (i / 2) * 8 + j;
                out[index] = row[j];
            }
        }
        return out;
    }




    } // namespace bitmanip