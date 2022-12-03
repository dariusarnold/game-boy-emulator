#include "emulator.hpp"
#include "addressbus.hpp"
#include "bootrom.hpp"
#include "cartridge.hpp"
#include "cpu.hpp"
#include "exceptions.hpp"
#include "gpu.hpp"
#include "ram.hpp"
#include "timer.hpp"

#include "spdlog/spdlog.h"


Emulator::Emulator(const std::array<uint8_t, 256>& boot_rom, const std::vector<uint8_t>& game_rom) :
        m_cartridge(std::make_shared<Cartridge>(this, game_rom)),
        m_boot_rom(std::make_shared<BootRom>(this, boot_rom)),
        m_address_bus(std::make_shared<AddressBus>(this)),
        m_ram(std::make_shared<Ram>(this)),
        m_cpu(std::make_shared<Cpu>(this)),
        m_gpu(std::make_shared<Gpu>()),
        m_interrupt_handler(std::make_shared<InterruptHandler>(this)),
        m_timer(std::make_shared<Timer>(this)),
        m_logger(spdlog::get("")) {}

Emulator::Emulator(const std::vector<uint8_t>& game_rom) :
        m_cartridge(std::make_shared<Cartridge>(this, game_rom)),
        m_address_bus(std::make_shared<AddressBus>(this)),
        m_ram(std::make_shared<Ram>(this)),
        m_cpu(std::make_shared<Cpu>(this)),
        m_gpu(std::make_shared<Gpu>()),
        m_interrupt_handler(std::make_shared<InterruptHandler>(this)),
        m_timer(std::make_shared<Timer>(this)),
        m_logger(spdlog::get("")) {
    m_state.is_booting = false;
    m_cpu->set_initial_state();
}


void Emulator::run() {
    try {
        m_cpu->run();
        m_interrupt_handler->handle_interrupts();
    } catch (const std::exception& error) {
        m_logger->error("{} - CPU state {}", error.what(), m_cpu->get_minimal_debug_state());
        return;
    }
}

std::shared_ptr<AddressBus> Emulator::get_bus() const {
    return m_address_bus;
}

std::shared_ptr<Ram> Emulator::get_ram() const {
    return m_ram;
}

std::shared_ptr<BootRom> Emulator::get_boot_rom() const {
    return m_boot_rom;
}

bool Emulator::is_booting() const {
    return m_state.is_booting;
}

void Emulator::signal_boot_ended() {
    m_state.is_booting = false;
}

void Emulator::elapse_cycle() {
    m_state.cycles_m += 1;
    m_timer->cycle_elapsed_callback(m_state.cycles_m);
}

std::shared_ptr<Gpu> Emulator::get_gpu() const {
    return m_gpu;
}

std::string Emulator::get_cpu_debug_state() const {
    return m_cpu->get_minimal_debug_state();
}

bool Emulator::step() {
    return m_cpu->step();
}

opcodes::Instruction Emulator::get_current_instruction() const {
    return m_cpu->get_current_instruction();
}

opcodes::Instruction Emulator::get_previous_instruction() const {
    return m_cpu->get_previous_instruction();
}

std::shared_ptr<Cartridge> Emulator::get_cartridge() const {
    return m_cartridge;
}

void Emulator::set_interrupts_enabled(bool enabled) {
    m_interrupt_handler->set_global_interrupt_enabled(enabled);
}

void Emulator::elapse_instruction() {
    m_state.instructions_executed++;
    m_logger->debug("{} instructions elapsed", m_state.instructions_executed);
    m_interrupt_handler->callback_instruction_elapsed();
}

std::shared_ptr<InterruptHandler> Emulator::get_interrupt_handler() const {
    return m_interrupt_handler;
}

std::shared_ptr<Cpu> Emulator::get_cpu() const {
    return m_cpu;
}

std::shared_ptr<Timer> Emulator::get_timer() const {
    return m_timer;
}
