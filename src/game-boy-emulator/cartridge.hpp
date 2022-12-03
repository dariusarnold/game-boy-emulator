#pragma once

class Emulator;
#include <vector>
#include <cstdint>
#include <cstddef>


class Cartridge {
public:
    Cartridge(Emulator* emulator, std::vector<uint8_t> rom);

    [[nodiscard]] uint8_t read_byte(uint16_t address) const;
    void write_byte(uint16_t address, uint8_t value);

private:
    struct RomInfo {
        int size_bytes = 0;
        int num_banks = 0;
    };
    struct RamInfo {
        int size_bytes = 0;
        int num_banks = 0;
    };
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

    [[nodiscard]] Cartridge::CartridgeType get_cartridge_type() const;

    // Return ROM size on cartridge in bytes.
    [[nodiscard]] RomInfo get_rom_size() const;
    [[nodiscard]] RamInfo get_ram_size() const;

    void write_registers(uint16_t address, uint8_t value);
    void write_values(uint16_t address, uint8_t value);

    std::vector<uint8_t> m_rom;
    Emulator* m_emulator;
    CartridgeType m_cartridge_type;
    RomInfo m_rom_size_info;
    RamInfo m_ram_size_info;

    // Registers
    // RAM Enable (0x0000-0x1FFF)
    bool m_ram_enable = false;
    // Only 5 bits, (0x2000-0x3FFF)
    uint8_t m_bank1 = 0b00001;
    // Only 2 bits, (0x4000-0x5FFF)
    uint8_t m_bank2 = 0;
    // Only 1 bit. Decides behaviour of RAM bank number. (0x6000-0x7FFF)
    uint8_t m_banking_mode_select = 0;
};
