#include "emulator.hpp"
#include "addressbus.hpp"
#include "apu.hpp"
#include "bootrom.hpp"
#include "cpu.hpp"
#include "exceptions.hpp"
#include "gpu.hpp"
#include "mmu.hpp"
#include "ram.hpp"


Emulator::Emulator(const std::array<uint8_t, 256>& boot_rom, const std::vector<uint8_t>& game_rom) :
        m_boot_rom(std::make_shared<BootRom>(this, boot_rom)),
        m_game_rom(game_rom),
        m_address_bus(std::make_shared<AddressBus>(this)),
        m_ram(std::make_shared<Ram>(this)),
        m_cpu(std::make_shared<Cpu>(this)),
        m_apu(std::make_shared<Apu>()),
        m_gpu(std::make_shared<Gpu>()) {}

void Emulator::run() {
    try {
        //    m_mmu->map_cartridge(m_game_rom);
        //        m_mmu->map_boot_rom(m_boot_rom);
        //    m_mmu->map_memory_range(m_gpu->get_mappable_memory());
        //    m_mmu->map_memory_range(m_apu->get_mappable_memory());
        //    m_mmu->map_memory_range(m_cpu->get_mappable_memory());
        m_cpu->run();
    } catch (const LogicError& error) {
        abort_execution(error.what());
    } catch (const NotImplementedError& error) {
        abort_execution(error.what());
    }
}

// std::shared_ptr<Mmu> Emulator::get_mmu() const {
//     return m_mmu;
// }

std::shared_ptr<AddressBus> Emulator::get_bus() const {
    return m_address_bus;
}

void Emulator::abort_execution(std::string error_msg) {
    fmt::print("{} - CPU state {}",error_msg, m_cpu->get_minimal_debug_state());
    std::exit(1);
}

std::shared_ptr<Ram> Emulator::get_ram() const {
    return m_ram;
}

std::shared_ptr<BootRom> Emulator::get_boot_rom() const {
    return m_boot_rom;
}

bool Emulator::is_booting() const {
    return m_is_booting;
}

void Emulator::signal_boot_ended() {
    m_is_booting = false;
}

void Emulator::elapse_cycles(size_t cycles) {
    m_state.cycles += cycles;
}

std::shared_ptr<Gpu> Emulator::get_gpu() const {
    return m_gpu;
}
