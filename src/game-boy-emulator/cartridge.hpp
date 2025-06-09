#pragma once

class Emulator;
namespace spdlog {
class logger;
}
class MemoryMappedFile;
class Mbc;
#include <memory>
#include <vector>

namespace cartridge {

    // The cartridge header contains a field describing the type of PCB. Different PCBs have different chips and
    // different capabilities.
    // For example, memory bank controllers (MBCs) are used to expand the available address space by bank
    // switching.
    // See https://gbdev.io/pandocs/The_Cartridge_Header.html#0147--cartridge-type for values
    enum class CartridgeType {
        ROM_ONLY = 0x0,
        MBC1 = 0x1,
        MBC1_RAM = 0x2,
        MBC1_RAM_BATTERY = 0x3,
        MBC2 = 0x05,
        MBC2_BATTERY = 0x6,
        ROM_RAM = 0x8,
        ROM_RAM_BATTERY = 0x9,
        MMM01 = 0xB,
        MMM01_RAM = 0xC,
        MMM01_RAM_BATTERY = 0xD,
        MBC3_TIMER_BATTERY = 0x0F,
        MBC3_TIMER_RAM_BATTERY = 0x10,
        MBC3 = 0x11,
        MBC3_RAM = 0x12,
        MBC3_RAM_BATTERY = 0x13,
        MBC5 = 0x19,
        MBC5_RAM = 0x1A,
        MBC5_RAM_BATTERY = 0x1B,
        MBC5_RUMBLE = 0x1C,
        MBC5_RUMBLE_RAM = 0x1D,
        MBC5_RUMBLE_RAM_BATTERY = 0x1E,
        MBC6 = 0x20,
        MBC7_SENSOR_RUMBLE_RAM_BATTERY = 0x22,
        POCKET_CAMERA = 0xFC,
        BANDAI_TAMA5 = 0xFD,
        HUC3 = 0xFE,
        HUC1_RAM_BATTERY = 0xFF,
    };

    class Cartridge {
    public:
        Cartridge(Emulator* emulator, std::vector<uint8_t> rom);
        ~Cartridge();
        // Sadly, to keep the forward declarations for Mbc and MemoryMappedFile, a destructor for
        // Cartridge is required to be defined. This triggers warnings about the rule of five, to
        // silence those we have to manually define the other functions.
        Cartridge(const Cartridge&) = delete;
        Cartridge& operator=(const Cartridge&) = delete;
        Cartridge(Cartridge&&) = default;
        Cartridge& operator=(Cartridge&&) = default;

        [[nodiscard]] uint8_t read_byte(uint16_t address) const;
        void write_byte(uint16_t address, uint8_t value);

        // Write memory mapped ram contents to disk.
        void sync();

    private:
        Emulator* m_emulator;
        std::shared_ptr<spdlog::logger> m_logger;
        CartridgeType m_cartridge_type;
        std::unique_ptr<Mbc> m_mbc;
        std::unique_ptr<MemoryMappedFile> m_ram_file;
    };

    [[nodiscard]] std::string get_title(const std::vector<uint8_t>& rom);

    [[nodiscard]] CartridgeType get_type(const std::vector<uint8_t>& rom);
}