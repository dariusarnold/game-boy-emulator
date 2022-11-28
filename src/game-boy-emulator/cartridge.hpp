#pragma once

class Emulator;
#include <vector>
#include <cstdint>


class Cartridge {
public:
    Cartridge(Emulator* emulator, std::vector<uint8_t> rom);

    [[nodiscard]] uint8_t read_byte(uint16_t address) const;


private:
    std::vector<uint8_t> m_rom;
    Emulator* m_emulator;
};
