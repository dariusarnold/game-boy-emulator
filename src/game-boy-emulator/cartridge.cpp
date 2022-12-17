#include "cartridge.hpp"

#include "emulator.hpp"
#include "memorymap.hpp"
#include "exceptions.hpp"
#include "bitmanipulation.hpp"
#include "mbc.hpp"
#include "mbc1.hpp"
#include "mbc3.hpp"

#include "fmt/format.h"
#include "magic_enum.hpp"

#include <utility>
#include <spdlog/spdlog.h>

namespace {
const int CARTRIDGE_TYPE_OFFSET = 0x147;
const int ROM_SIZE = 0x148;
const int RAM_SIZE = 0x149;
} // namespace

Cartridge::Cartridge(Emulator* emulator, std::vector<uint8_t> rom) :
        m_emulator(emulator), m_logger(spdlog::get("")) {
    if (rom.size() < memmap::CartridgeHeaderEnd) {
        throw LogicError(
            fmt::format("ROM only {} bytes, does not contain cartridge header", rom.size()));
    }
    auto ram_size_info = get_ram_size_info(rom);

    // Initialize after size check to avoid potential out-of-bounds access.
    m_cartridge_type = get_cartridge_type(rom);
    m_logger->info("Detected MBC type {}", magic_enum::enum_name(m_cartridge_type));
    switch (m_cartridge_type) {
    case CartridgeType::MBC1:
    case CartridgeType::MBC1_RAM:
        m_mbc = std::make_unique<Mbc1>(std::move(rom), ram_size_info.size_bytes);
        break;
    case CartridgeType::MBC3:
    case CartridgeType::MBC3_RAM:
    case CartridgeType::MBC3_RAM_BATTERY:
    case CartridgeType::MBC3_TIMER_RAM_BATTERY:
        m_mbc = std::make_unique<Mbc3>(std::move(rom), ram_size_info.size_bytes);
        break;
    default:
        throw NotImplementedError(fmt::format("Cartridge type {} not implemented",
                                              magic_enum::enum_name(m_cartridge_type)));
    }
}

uint8_t Cartridge::read_byte(uint16_t address) const {
    return m_mbc->read_byte(address);
}

void Cartridge::write_byte(uint16_t address, uint8_t value) {
    m_mbc->write_byte(address, value);
};

Cartridge::CartridgeType Cartridge::get_cartridge_type(const std::vector<uint8_t>& rom) {
    auto val = rom[CARTRIDGE_TYPE_OFFSET];
    if (magic_enum::enum_contains<CartridgeType>(val)) {
        return CartridgeType{val};
    }
    throw LogicError(fmt::format("Invalid value {:02X} for cartridge type", val));
}

Cartridge::RomInfo Cartridge::get_rom_size_info(const std::vector<uint8_t>& rom) const {
    auto val = rom[ROM_SIZE];
    if (val > 0x8) {
        throw LogicError("Invalid value for ROM size");
    }
    return {(32 * 1024) * (1 << val), 1 << val};
}

Cartridge::RamInfo Cartridge::get_ram_size_info(const std::vector<uint8_t>& rom) const {
    auto val = rom[RAM_SIZE];
    switch (val) {
    case 0:
        return {0, 0};
    case 2:
        return {8 * 104, 1};
    case 3:
        return {32 * 1024, 4};
    case 4:
        return {128 * 1024, 16};
    case 5:
        return {64 * 1024, 8};
    default:
        throw LogicError("Invalid value for RAM size");
    }
}
