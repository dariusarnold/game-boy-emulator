#pragma once

#include "constants.h"
#include "options.hpp"
#include "graphics.hpp"
#include <array>
#include <cstdint>
#include <memory>
#include <vector>
#include <functional>
#include <filesystem>

namespace opcodes {
struct Instruction;
} // namespace opcodes
class Ram;
class Mmu;
class Cpu;
class Apu;
class Ppu;
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
    // Number of frames rendered
    size_t frame_count = 0;
    // Currently running boot rom
    bool is_booting = true;
    bool halted = false;
    // Path to game rom file
    std::optional<std::filesystem::path> rom_file_path;
    // When changing the game the new path is stored here before the new game is loaded.
    std::optional<std::filesystem::path> new_rom_file_path;
    std::string game_title;
};

/**
 * Main class for emulation, instantiates all parts, connects and manages them.
 */
class Emulator {
public:
    explicit Emulator(EmulatorOptions options);
    // Only load boot rom file.
    void load_boot(const std::filesystem::path& rom_path);
    // Don't run boot rom and use initial values for registers/flags/memory.
    void load_game(const std::filesystem::path& rom_path);
    // Actually run boot rom to initialize emulator and hand off control to game after booting.
    void load_boot_game(const std::filesystem::path& boot_rom_path,
                        const std::filesystem::path& game_rom_path);

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
    [[nodiscard]] EmulatorOptions& get_options();
    [[nodiscard]] const EmulatorState& get_state() const;
    EmulatorState& get_state();

    [[nodiscard]] std::shared_ptr<AddressBus> get_bus() const;
    [[nodiscard]] std::shared_ptr<Ram> get_ram() const;
    [[nodiscard]] std::shared_ptr<BootRom> get_boot_rom() const;
    [[nodiscard]] std::shared_ptr<Ppu> get_ppu() const;
    [[nodiscard]] std::shared_ptr<Cpu> get_cpu() const;
    [[nodiscard]] std::shared_ptr<Cartridge> get_cartridge() const;
    [[nodiscard]] std::shared_ptr<InterruptHandler> get_interrupt_handler() const;
    [[nodiscard]] std::shared_ptr<Timer> get_timer() const;
    [[nodiscard]] std::shared_ptr<SerialPort> get_serial_port() const;
    [[nodiscard]] std::shared_ptr<Apu> get_apu() const;
    [[nodiscard]] std::shared_ptr<Joypad> get_joypad() const;

    void draw();
    void set_draw_function(std::function<void()> f);
    void debug();
    void set_debug_function(std::function<void()> f);

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
    std::shared_ptr<Ppu> m_ppu;
    std::shared_ptr<InterruptHandler> m_interrupt_handler;
    std::shared_ptr<Timer> m_timer;
    std::shared_ptr<SerialPort> m_serial_port;
    std::shared_ptr<Joypad> m_joypad;
    std::shared_ptr<spdlog::logger> m_logger;

    // Function which is called on every VBlank. Can be used to draw the game framebuffer.
    std::function<void()> m_draw_function;
    // Function which is called on LD B,B instruction, which is used sort of as a debug
    // breakpoint.
    std::function<void()> m_debug_function;
};
