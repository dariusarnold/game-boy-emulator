#pragma once

#include <cstdint>
#include <cstddef>
#include <type_traits>

/**
 * All sizes are in bytes.
 */

namespace constants {

constexpr size_t BOOT_ROM_SIZE = 256;

constexpr size_t RAM_SIZE = 65536;

// Exemptions from above comment: size in bits
constexpr int NIBBLE_SIZE = 4;
constexpr int BYTE_SIZE = 8;

constexpr size_t SCREEN_RES_WIDTH = 160;
constexpr size_t SCREEN_RES_HEIGHT = 144;

// Bytes representing a 8x8 tile
constexpr int BYTES_PER_TILE = 16;

constexpr size_t BACKGROUND_SIZE_PIXELS = 256;

constexpr int VIEWPORT_WIDTH = 160;
constexpr int VIEWPORT_HEIGHT = 144;

} // namespace constants


enum class flags : uint8_t { zero = 7, subtract = 6, half_carry = 5, carry = 4 };


enum class BitValues : uint8_t { Active = 1, Inactive = 0 };


/**
 * Cast enum value to integral value of underlying type.
 */
template <typename Enum>
constexpr auto as_integral(Enum e) -> typename std::underlying_type_t<Enum> {
    return static_cast<typename std::underlying_type_t<Enum>>(e);
}
