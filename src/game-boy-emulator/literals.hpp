#pragma once


inline consteval uint8_t operator"" _u8(unsigned long long x) {
    return x > std::numeric_limits<uint8_t>::max()
               ? throw std::domain_error("Overflow in _u8 integer literal")
               : static_cast<uint8_t>(x);
}

inline consteval uint16_t operator"" _u16(unsigned long long x) {
    return x > std::numeric_limits<uint16_t>::max()
               ? throw std::domain_error("Overflow in _u16 integer literal")
               : static_cast<uint16_t>(x);
}