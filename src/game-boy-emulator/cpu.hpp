#pragma once

#include "bitmanipulation.hpp"
#include "constants.h"
#include "facades/flag.hpp"
#include "facades/incrementpc.hpp"
#include "facades/register.hpp"
#include "mmu.hpp"
#include "opcodes.hpp"

#include "fmt/format.h"

#include <algorithm>
#include <array>
#include <cstdint>


// TODO move this to registers.hpp
/**
 * Main data structure for register data.
 * Unions allow easy access to the lower and higher half of a two byte register.
 * This works on little endian only, for big endian the order of bytes has to be
 * swapped.
 */
struct Registers {
    uint16_t sp, pc;
    union {
        uint16_t af;
        struct {
            uint8_t f, a;
        };
    };
    union {
        uint16_t bc;
        struct {
            uint8_t c, b;
        };
    };
    union {
        uint16_t de;
        struct {
            uint8_t e, d;
        };
    };
    union {
        uint16_t hl;
        struct {
            uint8_t l, h;
        };
    };
};


/**
 * Formatted output for Registers.
 * Allows to print single registers by specifying the name in lower case.
 * Example: {:sp} would print the stack pointer
 */
template <>
struct fmt::formatter<Registers> {

    std::string format_string;

    /**
     * Parse until closing curly brace '}' is hit.
     * @param context Contains format string after :, meaning '{:f}' becomes 'f}'.
     * @return iterator past the end of the parsed range
     */
    auto parse(fmt::format_parse_context& context) {
        if (context.begin() == context.end()) {
            // No format string was specified
            return context.begin();
        }
        auto it = std::find(context.begin(), context.end(), '}');
        if (it == context.end()) {
            throw fmt::format_error("Invalid format");
        } else {
            format_string.assign(context.begin(), it);
        }
        return it;
    }

    template <typename FormatContext>
    auto format(const Registers& registers, FormatContext& context) {
        if (format_string == "sp") {
            return format_to(context.out(), "sp: {:02X}", registers.sp);
        } else if (format_string == "pc") {
            return format_to(context.out(), "pc: {:02X}", registers.pc);
        } else {
            throw fmt::format_error(fmt::format("Format string {} not supported", format_string));
        }
    }
};

class Cpu {
    Registers registers = {0};
    Mmu mmu;
    // Number of cycles since execution start
    size_t cycles = 0;
    // Number of instructions executed since start
    size_t instructions_executed = 0;

    /**
     * Function which modifies CPU state according to the instruction and returns the number of
     * cycles.
     */
    using Instruction = std::function<int()>;
    // Maps op code to function which executes the instruction associated with
    // the opcode
    std::unordered_map<opcodes::OpCode, Instruction> instructions;

public:
    Cpu();

    /**
     * Copy boot rom to first 256 bytes of ram.
     */
    void set_boot_rom(const std::array<uint8_t, constants::BOOT_ROM_SIZE>& boot_rom);

    /**
     * Start cpu.
     * Only call after boot rom was set.
     */
    void run();

private:
    /**
     * Execute single step. Returns false if opcode at current program counter is invalid, else true
     */
    bool step();

    template <typename T>
    MutableRegister<T> make_mutable_register() {
        if constexpr (std::is_same_v<T, registers::A>) {
            return MutableRegister<T>{registers.a};
        } else if constexpr (std::is_same_v<T, registers::B>) {
            return MutableRegister<T>{registers.b};
        } else if constexpr (std::is_same_v<T, registers::C>) {
            return MutableRegister<T>{registers.c};
        } else if constexpr (std::is_same_v<T, registers::D>) {
            return MutableRegister<T>{registers.d};
        } else if constexpr (std::is_same_v<T, registers::E>) {
            return MutableRegister<T>{registers.e};
        } else if constexpr (std::is_same_v<T, registers::F>) {
            return MutableRegister<T>{registers.f};
        } else if constexpr (std::is_same_v<T, registers::H>) {
            return MutableRegister<T>{registers.h};
        } else if constexpr (std::is_same_v<T, registers::L>) {
            return MutableRegister<T>{registers.l};
        } else if constexpr (std::is_same_v<T, registers::SP>) {
            return MutableRegister<T>{registers.sp};
        } else if constexpr (std::is_same_v<T, registers::PC>) {
            return MutableRegister<T>{registers.pc};
        } else if constexpr (std::is_same_v<T, registers::AF>) {
            return MutableRegister<T>{registers.af};
        } else if constexpr (std::is_same_v<T, registers::BC>) {
            return MutableRegister<T>{registers.bc};
        } else if constexpr (std::is_same_v<T, registers::DE>) {
            return MutableRegister<T>{registers.de};
        } else if constexpr (std::is_same_v<T, registers::HL>) {
            return MutableRegister<T>{registers.hl};
        }
    }

    template <flags f>
    MutableFlag<f> make_mutable_flag() {
        static_assert(f == flags::zero or f == flags::half_carry or f == flags::carry or
                      f == flags::subtract);
        return MutableFlag<f>{registers.f};
    }

    IncrementPC make_pc_incrementer() {
        return IncrementPC{make_mutable_register<registers::PC>()};
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
     * Common function for all 8 byte loads (LD).
     * @param input Reference to register into which to load the immediate value
     */
    void ld8(uint8_t& input);

    /**
     * Common function for all 16 byte loads (LD).
     * @param input Reference to register into which to load the immediate value
     */
    void ld16(uint16_t& input);

    /**
     * Common function for loading/storing A from/to adress pointed at by HL and
     * incrementing/decrementing HL after.
     * @param op Opcode, used to decide wether to load/store and increment/decrement.
     * @return Number of cycles for instruction
     */
    int indirect_hl(opcodes::OpCode op);

    /**
     * Handle second byte of two byte instruction codes beginning with prefix cb
     * @param op_code second byte of opcode
     */
    uint8_t cb(opcodes::OpCode op_code);

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
    uint8_t op_code_to_register(opcodes::OpCode opcode);

    /**
     * Write value to register specified by the opcode. Used for the second byte of CB instruction.
     */
    void write_to_destionation(opcodes::OpCode destination, uint8_t value);

    /**
     * Common function for all jumps.
     * @param is_condition_met if true read immediate value and set stackpointer to it.
     */
    void jump_r(bool is_condition_met);

    /**
     * Save value to address pointed at by register hl and increment program counter
     * @param value
     */
    int save_value_to_address(uint16_t address, uint8_t value);
};

namespace internal {
    /**
     * Resolve second byte of CB opcodes to bit position on which this instructions operates.
     */
    uint8_t op_code_to_bit(opcodes::OpCode opcode);
} // namespace internal
