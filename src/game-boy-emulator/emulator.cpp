#include "emulator.hpp"
#include "cpu.hpp"
#include "mmu.hpp"
#include "gpu.hpp"
#include "apu.hpp"


Emulator::Emulator(const std::array<uint8_t, 256>& boot_rom, const std::vector<uint8_t>& game_rom) :
        m_boot_rom(boot_rom), m_game_rom(game_rom) {}

void Emulator::run() {
    Mmu mmu;
    Gpu gpu;
    Apu apu;
    mmu.map_cartridge(m_game_rom);
    mmu.map_boot_rom(m_boot_rom);
    mmu.map_memory_range(gpu.get_mappable_memory());
    mmu.map_memory_range(apu.get_mappable_memory());
    Cpu cpu(mmu);
    mmu.map_memory_range(cpu.get_mappable_memory());
    cpu.run();
}
