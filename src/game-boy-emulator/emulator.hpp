#pragma once

#include "constants.h"
#include "options.hpp"
#include <array>
#include <cstdint>
#include <memory>
#include <vector>

namespace opcodes {
struct Instruction;
} // namespace opcodes
class Ram;
class Mmu;
class Cpu;
class Apu;
class Gpu;
class AddressBus;
class BootRom;
class Cartridge;
class Timer;
class SerialPort;
class InterruptHandler;
namespace spdlog {
class logger;
}
struct CpuDebugState;
class Joypad;

struct EmulatorState {
    // Number of m cycles since execution start
    size_t cycles_m = 0;
    // Number of instructions since execution start
    size_t instructions_executed = 0;
    // Currently running boot rom
    bool is_booting = true;
    bool halted = false;
};

/**
 * Main class for emulation, instantiates all parts, connects and manages them.
 */
class Emulator {
public:
    // Actually run boot rom to initialize emulator and hand off control to game after booting.
    Emulator(const std::array<uint8_t, constants::BOOT_ROM_SIZE>& boot_rom,
             const std::vector<uint8_t>& game_rom, EmulatorOptions options);
    // Don't run boot rom and use initial values for registers/flags/memory.
    explicit Emulator(const std::vector<uint8_t>& game_rom, EmulatorOptions options);
    void run();
    bool step();

    [[nodiscard]] bool is_booting() const;
    void signal_boot_ended();
    void elapse_instruction();
    void elapse_cycle();
    void set_interrupts_enabled(bool enabled);
    void halt();
    void unhalt();

    [[nodiscard]] std::string get_cpu_debug_state() const;
    [[nodiscard]] CpuDebugState get_debug_state() const;
    [[nodiscard]] opcodes::Instruction get_current_instruction() const;
    [[nodiscard]] opcodes::Instruction get_previous_instruction() const;

    [[nodiscard]] const EmulatorOptions& get_options() const;
    [[nodiscard]] const EmulatorState& get_state() const;

    [[nodiscard]] std::shared_ptr<AddressBus> get_bus() const;
    [[nodiscard]] std::shared_ptr<Ram> get_ram() const;
    [[nodiscard]] std::shared_ptr<BootRom> get_boot_rom() const;
    [[nodiscard]] std::shared_ptr<Gpu> get_gpu() const;
    [[nodiscard]] std::shared_ptr<Cpu> get_cpu() const;
    [[nodiscard]] std::shared_ptr<Cartridge> get_cartridge() const;
    [[nodiscard]] std::shared_ptr<InterruptHandler> get_interrupt_handler() const;
    [[nodiscard]] std::shared_ptr<Timer> get_timer() const;
    [[nodiscard]] std::shared_ptr<SerialPort> get_serial_port() const;
    [[nodiscard]] std::shared_ptr<Apu> get_apu() const;
    [[nodiscard]] std::shared_ptr<Joypad> get_joypad() const;

    size_t get_cycle_count();

private:
    EmulatorState m_state;
    EmulatorOptions m_options;
    std::shared_ptr<Cartridge> m_cartridge;
    std::shared_ptr<BootRom> m_boot_rom;
    std::shared_ptr<AddressBus> m_address_bus;
    std::shared_ptr<Ram> m_ram;
    std::shared_ptr<Cpu> m_cpu;
    std::shared_ptr<Apu> m_apu;
    std::shared_ptr<Gpu> m_gpu;
    std::shared_ptr<InterruptHandler> m_interrupt_handler;
    std::shared_ptr<Timer> m_timer;
    std::shared_ptr<SerialPort> m_serial_port;
    std::shared_ptr<Joypad> m_joypad;
    std::shared_ptr<spdlog::logger> m_logger;
};
