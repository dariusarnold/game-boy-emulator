#include "emulator.hpp"
#include "apu.hpp"
#include "cpu.hpp"
#include "gpu.hpp"
#include "mmu.hpp"
#include "addressbus.hpp"


Emulator::Emulator(const std::array<uint8_t, 256>& boot_rom, const std::vector<uint8_t>& game_rom) :
        m_boot_rom(boot_rom),
        m_game_rom(game_rom),
        m_address_bus(std::make_shared<AddressBus>(this)),
        m_mmu(std::make_shared<Mmu>()),
        m_cpu(std::make_shared<Cpu>(this)),
        m_apu(std::make_shared<Apu>()),
        m_gpu(std::make_shared<Gpu>()) {}

void Emulator::run() {
    m_mmu->map_cartridge(m_game_rom);
    m_mmu->map_boot_rom(m_boot_rom);
    m_mmu->map_memory_range(m_gpu->get_mappable_memory());
    m_mmu->map_memory_range(m_apu->get_mappable_memory());
    m_mmu->map_memory_range(m_cpu->get_mappable_memory());
    m_cpu->run();
}

std::shared_ptr<Mmu> Emulator::get_mmu() const {
    return m_mmu;
}

std::shared_ptr<AddressBus> Emulator::get_bus() const {
    return m_address_bus;
}

void Emulator::abort_execution(std::string error_msg) {
    // TODO Dump current state
    throw std::runtime_error(error_msg);
}
