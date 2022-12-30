#include "cartridge.hpp"

#include "memorymappedfile.hpp"
#include "emulator.hpp"
#include "memorymap.hpp"
#include "exceptions.hpp"
#include "bitmanipulation.hpp"
#include "nombc.hpp"
#include "mbc.hpp"
#include "mbc1.hpp"
#include "mbc3.hpp"
#include "mbc5.hpp"
#include "io.hpp"

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

    std::span<uint8_t> ram;
    if (ram_size_info.size_bytes != 0) {
        // Memory map cartridge RAM as a file to emulate the battery backed RAM.
        auto ram_file_path = m_emulator->get_state().rom_file_path.value();
        auto ram_file_name = ram_file_path.filename().string().append(".ram");
        ram_file_path = ram_file_path.replace_filename(ram_file_name);
        m_ram_file = std::make_unique<MemoryMappedFile>(ram_file_path, ram_size_info.size_bytes);
        m_logger->info("Opening {} as cartridge RAM file", ram_file_path.string());
        ram = m_ram_file->get_data();
    }

    auto rom_size = get_rom_size_info(rom);
    // Initialize after size check to avoid potential out-of-bounds access.
    m_cartridge_type = get_cartridge_type(rom);
    m_logger->info("Detected MBC type {}, ROM {} bytes, {} banks, RAM {} bytes, {} banks",
                   magic_enum::enum_name(m_cartridge_type), rom_size.size_bytes, rom_size.num_banks,
                   ram_size_info.size_bytes, ram_size_info.num_banks);
    switch (m_cartridge_type) {
    case CartridgeType::ROM_ONLY:
        if (rom.size() != memmap::CartridgeRomSize) {
            throw LogicError("Invalid ROM size");
        }
        m_mbc = std::make_unique<NoMbc>(std::move(rom), ram);
        break;
    case CartridgeType::MBC1:
    case CartridgeType::MBC1_RAM:
    case CartridgeType::MBC1_RAM_BATTERY:
        m_mbc = std::make_unique<Mbc1>(std::move(rom), ram);
        break;
    case CartridgeType::MBC3:
    case CartridgeType::MBC3_RAM:
    case CartridgeType::MBC3_RAM_BATTERY:
    case CartridgeType::MBC3_TIMER_RAM_BATTERY:
        m_mbc = std::make_unique<Mbc3>(std::move(rom), ram);
        break;
    case CartridgeType::MBC5:
    case CartridgeType::MBC5_RAM:
    case CartridgeType::MBC5_RAM_BATTERY:
    case CartridgeType::MBC5_RUMBLE_RAM_BATTERY:
    case CartridgeType::MBC5_RUMBLE:
        m_mbc = std::make_unique<Mbc5>(std::move(rom), ram);
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
    return {static_cast<size_t>((32 * 1024) * (1 << val)), static_cast<size_t>(1 << val)};
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

void Cartridge::sync() {
    if (m_ram_file) {
        m_ram_file->sync();
    }
}

// We need a destructor for the outer class to be defined where the MemoryMappedFile is complete.
Cartridge::~Cartridge() = default;
