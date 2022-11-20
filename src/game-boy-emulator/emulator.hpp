#pragma once

#include <array>
#include <cstdint>
#include <memory>
#include <vector>

class Mmu;
class Cpu;
class Apu;
class Gpu;
class AddressBus;



/**
 * Main class for emulation, instantiates all parts, connects and manages them.
 */
class Emulator {
public:
    Emulator(const std::array<uint8_t, 256>& boot_rom, const std::vector<uint8_t>& game_rom);
    void run();

    void abort_execution(std::string error_msg);

    std::shared_ptr<Mmu> get_mmu() const;
    std::shared_ptr<AddressBus> get_bus() const;

private:

    std::array<uint8_t, 256> m_boot_rom;
    // TODO This should be its on class (Cartridge)
    std::vector<uint8_t> m_game_rom;

    std::shared_ptr<AddressBus> m_address_bus;
    std::shared_ptr<Mmu> m_mmu;
    std::shared_ptr<Cpu> m_cpu;
    std::shared_ptr<Apu> m_apu;
    std::shared_ptr<Gpu> m_gpu;
};