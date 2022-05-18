#include "emulator.hpp"
#include "cpu.hpp"

Emulator::Emulator(const std::array<uint8_t, 256>& boot_rom): m_boot_rom(boot_rom) {}

void Emulator::run() {
    Cpu cpu;
    cpu.set_boot_rom(m_boot_rom);
    cpu.run();
}
