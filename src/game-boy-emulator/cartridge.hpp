#pragma once

class Emulator;
namespace spdlog {
class logger;
}
class MemoryMappedFile;
#include "mbc.hpp"
#include <memory>
#include <vector>
#include <cstdint>
#include <cstddef>


class Cartridge {
public:
    Cartridge(Emulator* emulator, std::vector<uint8_t> rom);
    ~Cartridge();

    [[nodiscard]] uint8_t read_byte(uint16_t address) const;
    void write_byte(uint16_t address, uint8_t value);

    // Write memory mapped ram contents to disk.
    void sync();
private:
    // Memory bank controllers (MBCs) are used to expand the available address space by bank
    // switching.
    enum class CartridgeType {
        ROM_ONLY = 0x0,
        MBC1 = 0x1,
        MBC1_RAM = 0x2,
        MBC1_RAM_BATTERY = 0x3,
        MBC2 = 0x05,
        MBC2_BATTERY,
        ROM_RAM = 0x8,
        ROM_RAM_BATTERY,
        MMM01 = 0xB,
        MMM01_RAM,
        MMM01_RAM_BATTERY,
        MBC3_TIMER_BATTERY = 0x0F,
        MBC3_TIMER_RAM_BATTERY,
        MBC3,
        MBC3_RAM,
        MBC3_RAM_BATTERY,
        MBC5 = 0x19,
        MBC5_RAM = 0x1A,
        MBC5_RAM_BATTERY,
        MBC5_RUMBLE,
        MBC5_RUMBLE_RAM,
        MBC5_RUMBLE_RAM_BATTERY,
        MBC6 = 0x20,
        MBC7_SENSOR_RUMBLE_RAM_BATTERY = 0x22,
        POCKET_CAMERA = 0xFC,
        BANDAI_TAMA5,
        HUC3,
        HUC1_RAM_BATTERY,
    };

    [[nodiscard]] static Cartridge::CartridgeType
    get_cartridge_type(const std::vector<uint8_t>& rom);

    struct RomInfo {
        size_t size_bytes = 0;
        size_t num_banks = 0;
    };
    struct RamInfo {
        size_t size_bytes = 0;
        size_t num_banks = 0;
    };

    [[nodiscard]] RomInfo get_rom_size_info(const std::vector<uint8_t>& rom) const;
    [[nodiscard]] RamInfo get_ram_size_info(const std::vector<uint8_t>& rom) const;

    Emulator* m_emulator;
    std::shared_ptr<spdlog::logger> m_logger;
    CartridgeType m_cartridge_type;
    std::unique_ptr<Mbc> m_mbc;
    std::unique_ptr<MemoryMappedFile> m_ram_file;
};
