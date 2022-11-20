#pragma once

#include "bitmanipulation.hpp"
#include "constants.h"
#include "facades/flag.hpp"
#include "facades/programcounter.hpp"
#include "facades/register.hpp"
#include "opcodes.hpp"
#include "registers.hpp"
#include "interrupthandler.hpp"
#include "memory_range.hpp"

#include "fmt/format.h"

#include <algorithm>
#include <array>
#include <cstdint>


class Emulator;



// Typedef for clock cycles.
// Gives a more descriptive name but doesn't change semantics.
using t_cycle = size_t;

enum class Verbosity {
    LEVEL_NONE = 0,
    LEVEL_ERROR = 1,
    LEVEL_DEBUG = 2,
    LEVEL_INFO = 3,
};

class Cpu {
    Registers registers = {};
    Emulator* m_emulator;
    // Number of instructions executed since start
    size_t instructions_executed = 0;

    opcodes::Instruction current_instruction;
    opcodes::Instruction previous_instruction;

    /**
     * Function which modifies CPU state according to the instruction and returns the number of
     * cycles.
     */
    using Instruction = std::function<unsigned int()>;
    // Maps op code to function which executes the instruction associated with
    // the opcode
    std::unordered_map<opcodes::OpCode, Instruction> instructions;

    // Controlls logging to stdout
    Verbosity verbosity;

    InterruptHandler interrupt_handler;

    // Print considering the selected verbosity level
    void print(std::string_view message, Verbosity level);
public:
    explicit Cpu(Emulator* emulator);
    explicit Cpu(Emulator* emulator, Verbosity verbosity_);

    /**
     * Start cpu.
     * Only call after boot rom was set.
     */
    void run();

    /**
     * Execute single step. Returns false if opcode at current program counter is invalid, else true
     */
    bool step();

    std::vector<IMemoryRange*> get_mappable_memory();

    std::string get_minimal_debug_state();

    opcodes::Instruction get_current_instruction();
    opcodes::Instruction get_previous_instruction();

private:
    template <typename T>
    void abort_execution(std::string_view msg) {
        auto complete_msg
            = fmt::format("CPU ERROR: {}\nRan for {} instructions.", msg, instructions_executed);
        throw T{complete_msg};
    }

    template <typename T>
    Register<T> make_mutable_register() {
        if constexpr (std::is_same_v<T, registers::A>) {
            return Register<T>{registers.a};
        } else if constexpr (std::is_same_v<T, registers::B>) {
            return Register<T>{registers.b};
        } else if constexpr (std::is_same_v<T, registers::C>) {
            return Register<T>{registers.c};
        } else if constexpr (std::is_same_v<T, registers::D>) {
            return Register<T>{registers.d};
        } else if constexpr (std::is_same_v<T, registers::E>) {
            return Register<T>{registers.e};
        } else if constexpr (std::is_same_v<T, registers::F>) {
            return Register<T>{registers.f};
        } else if constexpr (std::is_same_v<T, registers::H>) {
            return Register<T>{registers.h};
        } else if constexpr (std::is_same_v<T, registers::L>) {
            return Register<T>{registers.l};
        } else if constexpr (std::is_same_v<T, registers::SP>) {
            return Register<T>{registers.sp};
        } else if constexpr (std::is_same_v<T, registers::PC>) {
            return Register<T>{registers.pc};
        } else if constexpr (std::is_same_v<T, registers::AF>) {
            return Register<T>{registers.af};
        } else if constexpr (std::is_same_v<T, registers::BC>) {
            return Register<T>{registers.bc};
        } else if constexpr (std::is_same_v<T, registers::DE>) {
            return Register<T>{registers.de};
        } else if constexpr (std::is_same_v<T, registers::HL>) {
            return Register<T>{registers.hl};
        }
    }

    template <flags f>
    MutableFlag<f> make_mutable_flag() {
        static_assert(f == flags::zero or f == flags::half_carry or f == flags::carry or
                      f == flags::subtract);
        return MutableFlag<f>{registers.f};
    }

    ProgramCounterIncDec make_pc_incrementer() {
        return ProgramCounterIncDec{make_mutable_register<registers::PC>()};
    }

    /**
     * Sets bit flag in flag register f
     */
    void set_zero_flag(BitValues value);
    void set_subtract_flag(BitValues value);
    void set_half_carry_flag(BitValues value);
    void set_carry_flag(BitValues value);

    /**
     * Return true if flag is set, otherwise return false.
     */
    [[nodiscard]] bool is_flag_set(flags flag) const;

    /**
     * Common function for all XOR instructions.
     * XORs input against value of a and stores result in a. Sets zero flag if a is zero after XOR.
     */
    void xor8(uint8_t input);

    /**
     * Common function for all 1 byte decrement instructions
     * @param input
     */
    void dec8(uint8_t& input);

    /**
     * Common function for all 8 byte loads (LD).
     * @param input Reference to register into which to load the immediate value
     */
    t_cycle ld8(uint8_t& input);

    /**
     * Common function for loading/storing A from/to adress pointed at by HL and
     * incrementing/decrementing HL after.
     * @param op Opcode, used to decide wether to load/store and increment/decrement.
     * @return Number of cycles for instruction
     */
    t_cycle indirect_hl(opcodes::OpCode op);

    /**
     * Handle second byte of two byte instruction codes beginning with prefix cb
     * @param instruction_byte second byte of opcode
     */
    t_cycle cb(uint8_t instruction_byte);

    /**
     * Base function for comparison opcodes
     * @param value
     * @param a
     * @return
     */
    t_cycle cp(uint8_t a, uint8_t value);

    /**
     * Base function for subtract from a opcodes
     * @param value
     * @return
     */
    t_cycle sub(uint8_t value);

    /**
     * Base function for add from a opcodes
     * @param value
     * @return
     */
    t_cycle add(uint8_t value);

    /**
     * Common function for all bit test instructions.
     * Test if bit at position in value is set and sets zero flag accordingly.
     * @param value
     * @param position
     */
    void test_bit(uint8_t value, u_int8_t position);

    /**
     * Convert second byte of cb opcodes to the register, upon which this instruction operators
     * @param opcode
     * @return
     */
    uint8_t op_code_to_register(uint8_t opcode_byte);

    /**
     * Write value to register specified by the opcode. Used for the second byte of CB instruction.
     */
    void write_to_destination(uint8_t opcode_byte, uint8_t value);

    /**
     * Common function for all jumps.
     * @param is_condition_met if true read immediate value and set stackpointer to it.
     */
    t_cycle jump_r(bool is_condition_met);

    /**
     * Save value to address
     * @param value
     */
    t_cycle save_value_to_address(uint16_t address, uint8_t value);

    t_cycle load_value_from_address(uint16_t address, uint8_t& value);


    /**
     * Read next instruction from address at program counter.
     * After this method reads the new opcode, the program counter will be incremented to point past
     * the opcode read
     * @return
     */
    opcodes::Instruction fetch_instruction();

    uint16_t fetch_data(opcodes::Instruction instruction);

    void set_register_value(opcodes::RegisterType register_type, uint16_t value);
    uint16_t get_register_value(opcodes::RegisterType register_type);

    void instructionLD(opcodes::Instruction instruction, uint16_t data);

    void instructionXOR(opcodes::Instruction instruction);
};

namespace internal {
    /**
     * Resolve second byte of CB opcodes to bit position on which this instructions operates.
     */
    uint8_t op_code_to_bit(uint8_t opcode_byte);

    template <typename F>
    bool was_half_carry(uint8_t a, uint8_t b, const F& operation) {
        bool hc = operation((a & 0xf), (b & 0xf)) & 0x10;
        return hc;
    }

    template <typename F>
    bool was_carry(uint8_t a, uint8_t b, const F& operation) {
        bool c = operation(static_cast<uint16_t>(a), static_cast<uint16_t>(b)) & 0x100;
        return c;
    }
} // namespace internal
