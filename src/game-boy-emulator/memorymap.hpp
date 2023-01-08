#pragma once

#include <cstdint>
#include <string_view>

/*
 * Contains constant definition for the memory map.
 */

namespace memmap {
// Single value address using one comparison for is_in
class Single {
    uint16_t m_address;

public:
    constexpr Single(uint16_t address,
                     uint16_t /* Only here so this has the same interface as Multi */) :
            m_address(address) {}

    [[nodiscard]] bool is_in(uint16_t address) const {
        return address == m_address;
    }

    [[nodiscard]] uint16_t size() const {
        return 1;
    }
};

// Address spanning a range using two comparisons for is_in
class Multi {
    uint16_t m_address_begin;
    uint16_t m_address_end;

public:
    constexpr Multi(uint16_t address_begin, uint16_t address_end) :
            m_address_begin(address_begin), m_address_end(address_end) {}
    [[nodiscard]] bool is_in(uint16_t address) const {
        return address >= m_address_begin && address <= m_address_end;
    }

    [[nodiscard]] uint16_t size() const {
        return m_address_end - m_address_begin + 1;
    }
};

// Statically forward to Multi or single
template <typename T>
struct AddressRange : public T {
    [[nodiscard]] bool is_in(uint16_t address) const {
        return T::is_in(address);
    }
    using T::T;
};


// Begin and End are both inclusive
#define X_MEMORY_MAP                                                                               \
    X(0x0100, 0x014F, CartridgeHeader)                                                             \
    X(0x0000, 0x00FF, RestartInterruptVectors)                                                     \
    X(0x0000, 0x00FF, BootRom)                                                                     \
    X(0x0000, 0x1FFF, RamEnable) /* Cartridge register */                                          \
    X(0x0000, 0x3FFF, CartridgeRomFixedBank)                                                       \
    X(0x0000, 0x7FFF, CartridgeRom)                                                                \
    X(0x0100, 0x104F, CardridgeHeader)                                                             \
    X(0x2000, 0x3FFF, RomBankNumber) /* Cartridge register */                                      \
    X(0x4000, 0x5FFF, RamBankNumber) /* Cartridge register */                                      \
    X(0x4000, 0x7FFF, CartridgeRomBankSwitchable)                                                  \
    X(0x6000, 0x7FFF, BankingModeSelect) /* Cartridge register */                                  \
    X(0x8000, 0x9FFF, VRam)                                                                        \
    X(0x8000, 0x97FF, TileData)                                                                    \
    X(0x9800, 0x9FFF, TileMaps)                                                                    \
    X(0x9800, 0x9BFF, TileMap1)                                                                    \
    X(0x9C00, 0x9FFF, TileMap2)                                                                    \
    X(0xA000, 0xBFFF, CartridgeRam)                                                                \
    X(0xC000, 0xDFFF, InternalRam)                                                                 \
    X(0xE000, 0xFDFF, EchoRam)                                                                     \
    X(0xFE00, 0xFE9F, OamRam)                                                                      \
    X(0xFF00, 0xFF7F, IORegisters)                                                                 \
    X(0xFF00, 0xFF00, Joypad)                                                                      \
    X(0xFF10, 0xFF26, Apu)                                                                         \
    X(0xFF40, 0xFF4B, PpuIoRegisters)                                                              \
    X(0xFF04, 0xFF07, Timer)                                                                       \
    X(0xFEA0, 0xFEFF, Prohibited)                                                                  \
    X(0xFF80, 0xFFFE, HighRam)                                                                     \
    X(0xFF0F, 0xFF0F, InterruptFlag)                                                               \
    X(0xFF01, 0xFF02, SerialPort)                                                                  \
    X(0xFF30, 0xFF3F, WavePattern)                                                                 \
    X(0xFF50, 0xFF50, DisableBootRom)                                                              \
    X(0xFFFF, 0xFFFF, InterruptEnable)


#define X(Begin_, End_, Name_)                                                                     \
    static_assert(Begin_ <= End_, #Name_ " range is invalid");                                     \
    constexpr std::integral_constant<uint16_t, Begin_> Name_##Begin;                               \
    constexpr std::integral_constant<uint16_t, End_> Name_##End;                                   \
    constexpr std::integral_constant<uint16_t, End_ - Begin_ + 1> Name_##Size;                     \
    constexpr AddressRange<std::conditional_t<Begin_ == End_, Single, Multi>> Name_(Begin_, End_);

X_MEMORY_MAP

#undef X

template <typename T>
inline bool is_in(uint16_t address, const AddressRange<T>& address_range) {
    return address_range.is_in(address);
}
} // namespace memmap
