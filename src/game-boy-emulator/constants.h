#pragma once

#include <cstdint>
#include <functional>

/**
 * All sizes are in bytes.
 */

namespace constants {

    const size_t BOOT_ROM_SIZE = 256;

    const size_t RAM_SIZE = 65536;

    // Exemptions from above comment: size in bits
    const int NIBBLE_SIZE = 4;
    const int BYTE_SIZE = 8;

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