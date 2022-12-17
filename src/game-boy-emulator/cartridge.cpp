#include "cartridge.hpp"

#include "emulator.hpp"
#include "memorymap.hpp"
#include "exceptions.hpp"
#include "bitmanipulation.hpp"
#include "mbc1.hpp"

#include "fmt/format.h"
#include "magic_enum.hpp"

#include <utility>
#include <spdlog/spdlog.h>

namespace {
const int CARTRIDGE_TYPE_OFFSET = 0x147;
}

Cartridge::Cartridge(Emulator* emulator, std::vector<uint8_t> rom) :
        m_emulator(emulator), m_logger(spdlog::get("")) {
    if (rom.size() < memmap::CartridgeHeaderEnd) {
        throw LogicError(
            fmt::format("ROM only {} bytes, does not contain cartridge header", rom.size()));
    }
    // Initialize after size check to avoid potential out-of-bounds access.
    m_cartridge_type = get_cartridge_type(rom);
    switch (m_cartridge_type) {
    case CartridgeType::ROM_ONLY:
    case CartridgeType::MBC1:
    case CartridgeType::MBC1_RAM:
        m_mbc = std::make_unique<Mbc1>(std::move(rom));
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

Mbc::Mbc(std::vector<uint8_t> rom) : m_rom(std::move(rom)), m_logger(spdlog::get("")) {}

std::vector<uint8_t>& Mbc::get_rom() {
    return m_rom;
}

const std::vector<uint8_t>& Mbc::get_rom() const {
    return m_rom;
}

std::shared_ptr<spdlog::logger> Mbc::get_logger() {
    return m_logger;
}

Mbc::~Mbc() = default;
