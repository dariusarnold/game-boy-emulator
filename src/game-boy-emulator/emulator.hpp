#pragma once

#include "constants.h"
#include "interrupthandler.hpp"
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
namespace spdlog {
class logger;
}

struct EmulatorState {
    // Number of m cycles since execution start
    size_t cycles = 0;
};

/**
 * Main class for emulation, instantiates all parts, connects and manages them.
 */
class Emulator {
public:
    Emulator(const std::array<uint8_t, constants::BOOT_ROM_SIZE>& boot_rom,
             const std::vector<uint8_t>& game_rom);
    void run();
    bool step();

    [[nodiscard]] bool is_booting() const;
    void signal_boot_ended();
    void elapse_instruction();
    void elapse_cycles(size_t cycles);
    void set_interrupts_enabled(bool enabled);

    [[nodiscard]] std::string get_cpu_debug_state() const;
    [[nodiscard]] opcodes::Instruction get_current_instruction() const;
    [[nodiscard]] opcodes::Instruction get_previous_instruction() const;

    [[nodiscard]] std::shared_ptr<AddressBus> get_bus() const;
    [[nodiscard]] std::shared_ptr<Ram> get_ram() const;
    [[nodiscard]] std::shared_ptr<BootRom> get_boot_rom() const;
    [[nodiscard]] std::shared_ptr<Gpu> get_gpu() const;
    [[nodiscard]] std::shared_ptr<Cpu> get_cpu() const;
    [[nodiscard]] std::shared_ptr<Cartridge> get_cartridge() const;
    [[nodiscard]] std::shared_ptr<InterruptHandler> get_interrupt_handler() const;

private:
    bool m_is_booting = true;
    size_t instructions_executed = 0;
    std::shared_ptr<Cartridge> m_cartridge;
    EmulatorState m_state;
    std::shared_ptr<BootRom> m_boot_rom;
    std::shared_ptr<AddressBus> m_address_bus;
    std::shared_ptr<Ram> m_ram;
    std::shared_ptr<Cpu> m_cpu;
    std::shared_ptr<Apu> m_apu;
    std::shared_ptr<Gpu> m_gpu;
    std::shared_ptr<InterruptHandler> m_interrupt_handler;
    std::shared_ptr<spdlog::logger> m_logger;
};
