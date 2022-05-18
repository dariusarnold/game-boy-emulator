#include "emulator.hpp"
#include "cpu.hpp"
#include "mmu.hpp"

Emulator::Emulator(const std::array<uint8_t, 256>& boot_rom): m_boot_rom(boot_rom) {}

void Emulator::run() {
    Mmu mmu;
    mmu.map_boot_rom(m_boot_rom);
    Cpu cpu(mmu);
    cpu.run();
}
