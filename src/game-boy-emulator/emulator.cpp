#include "emulator.hpp"
#include "addressbus.hpp"
#include "bootrom.hpp"
#include "cartridge.hpp"
#include "cpu.hpp"
#include "exceptions.hpp"
#include "ppu.hpp"
#include "ram.hpp"
#include "timer.hpp"
#include "serial_port.hpp"
#include "apu.hpp"
#include "interrupthandler.hpp"
#include "joypad.hpp"
#include "io.hpp"

#include "spdlog/spdlog.h"

Emulator::Emulator(EmulatorOptions options) :
        m_options(options),
        m_address_bus(std::make_shared<AddressBus>(this)),
        m_ram(std::make_shared<Ram>(this)),
        m_cpu(std::make_shared<Cpu>(this)),
        m_apu(std::make_shared<Apu>()),
        m_ppu(std::make_shared<Ppu>(this)),
        m_interrupt_handler(std::make_shared<InterruptHandler>(this)),
        m_timer(std::make_shared<Timer>(this)),
        m_serial_port(std::make_shared<SerialPort>(this)),
        m_joypad(std::make_shared<Joypad>(this)),
        m_logger(spdlog::get("")) {}

void Emulator::load_game(const std::filesystem::path& rom_path) {
    EmulatorIo io;
    auto rom = io.load_rom_file(rom_path);
    if (!rom.has_value()) {
        throw LoadError(fmt::format("Failed to load {}", rom_path.string()));
    }
    m_state.rom_file_path = rom_path;
    m_cartridge = std::make_shared<Cartridge>(this, rom.value());
    m_cpu->set_initial_state();
    m_state.is_booting = false;
}

void Emulator::load_boot(const std::filesystem::path& rom_path) {
    EmulatorIo io;
    auto boot_rom = io.load_boot_rom_file(rom_path);
    if (!boot_rom.has_value()) {
        throw LoadError(fmt::format("Failed to load {}", rom_path.string()));
    }
    m_boot_rom = std::make_shared<BootRom>(this, boot_rom.value());
    m_state.is_booting = true;
}

void Emulator::load_boot_game(const std::filesystem::path& boot_rom_path,
                              const std::filesystem::path& game_rom_path) {
    load_boot(boot_rom_path);
    EmulatorIo io;
    auto rom = io.load_rom_file(game_rom_path);
    if (!rom.has_value()) {
        throw LoadError(fmt::format("Failed to load {}", game_rom_path.string()));
    }
    m_state.rom_file_path = game_rom_path;
    m_cartridge = std::make_shared<Cartridge>(this, rom.value());
}

void Emulator::run() {
    while (step()) {
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
    m_ppu->cycle_elapsed_callback(m_state.cycles_m);
}

std::shared_ptr<Ppu> Emulator::get_ppu() const {
    return m_ppu;
}

std::string Emulator::get_cpu_debug_state() const {
    return m_cpu->get_minimal_debug_state();
}

CpuDebugState Emulator::get_debug_state() const {
    return m_cpu->get_debug_state();
}

bool Emulator::step() {
    try {
        if (!m_state.halted) {
            m_cpu->step();
        } else {
            elapse_cycle();
        }
        m_interrupt_handler->handle_interrupts();
        return true;
    } catch (const std::exception& e) {
        m_logger->error("{} - CPU state {}", e.what(), m_cpu->get_minimal_debug_state());
        return false;
    }
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

std::shared_ptr<SerialPort> Emulator::get_serial_port() const {
    return m_serial_port;
}

void Emulator::halt() {
    m_state.halted = true;
}

void Emulator::unhalt() {
    m_state.halted = false;
}

const EmulatorOptions& Emulator::get_options() const {
    return m_options;
}

EmulatorOptions& Emulator::get_options() {
    return m_options;
}

const EmulatorState& Emulator::get_state() const {
    return m_state;
}

std::shared_ptr<Apu> Emulator::get_apu() const {
    return m_apu;
}

std::shared_ptr<Joypad> Emulator::get_joypad() const {
    return m_joypad;
}

void Emulator::draw(const Framebuffer<graphics::gb::ColorScreen>& game) {
    m_cartridge->sync();
    m_draw_function(game);
}

void Emulator::set_draw_function(
    std::function<void(const Framebuffer<graphics::gb::ColorScreen>&)> f) {
    m_draw_function = f;
}

size_t Emulator::get_cycle_count() {
    return m_cpu->cycle_duration_previous_instruction();
}
