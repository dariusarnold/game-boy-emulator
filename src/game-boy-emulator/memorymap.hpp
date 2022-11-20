#pragma once

#include <cstdint>
#include <string_view>

/*
 * Contains constant definition for the memory map.
 */

// Begin and End are both inclusive
#define X_MEMORY_MAP                                                                               \
    X(0x0000, 0x0100, BootRom)                                                                     \
    X(0xC000, 0xCFFF, InternalRamBank0)                                                            \
    X(0xFF80, 0xFFFE, HighRam)

// Make Begin, End, Size and Name constants
#define X(Begin_, End_, Name_)                                                                       \
    constexpr std::pair<uint16_t, uint16_t> Name_{Begin_, End_};                                     \
    constexpr uint16_t Name_##Begin = Begin_;                                                       \
    constexpr uint16_t Name_##End = End_;                                                           \
    constexpr uint16_t Name_##Size = End_ - Begin_ + 1;                                              \
    constexpr std::string_view Name_##Name = #Name_;

namespace memmap {
X_MEMORY_MAP
}

#undef X

namespace memmap {
inline bool isIn(uint16_t address, uint16_t begin, uint16_t end) {
    return address >= begin && address <= end;
}
inline bool isIn(uint16_t address, std::pair<uint16_t, uint16_t> range) {
    return isIn(address, range.first, range.second);
}
} // namespace memmap