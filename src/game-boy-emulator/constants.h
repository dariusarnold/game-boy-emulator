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
constexpr size_t BYTES_PER_TILE = 16;
constexpr size_t PIXELS_PER_TILE = 8;

constexpr size_t BACKGROUND_SIZE_PIXELS = 256;

// Dimensions of the debug sprite viewer in sprites
constexpr int SPRITE_VIEWER_WIDTH = 16;
constexpr int SPRITE_VIEWER_HEIGHT = 8;

constexpr int OAM_DMA_NUM_BYTES = 160;

// CPU speed
constexpr int CLOCK_SPEED_T = 4194304;
// Speed in M cycles
constexpr int CLOCK_SPEED_M = CLOCK_SPEED_T / 4;

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
