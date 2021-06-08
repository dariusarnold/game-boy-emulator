#pragma once

#include "constants.h"
#include "fmt/format.h"
#include "opcodes.hpp"
#include <algorithm>
#include <array>
#include <cstdint>


// This works on little endian only, for big endian the order of bytes has to be swapped.
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
 * Formatted output for Registers
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
        return format_to(context.out(), "sp: {:02X}, pc: {:02x}", registers.sp, registers.pc);
    }
};


struct Ram {
    std::array<uint8_t, constants::RAM_SIZE> mem;
};


class Cpu {
    Registers registers;
    Ram ram;
    // Number of cycles since execution start
    size_t cycles = 0;

    /**
     * Function which modifies CPU state according to the instruction and returns the number of
     * cycles.
     */
    using Instruction = std::function<uint8_t()>;
    // Assigns executable function to opcode
    std::unordered_map<opcodes::OpCode, Instruction> instructions;

public:
    Cpu();

    /**
     * Copy boot rom to first 256 bytes of ram.
     */
    void set_boot_rom(const std::array<uint8_t, constants::BOOT_ROM_SIZE>& boot_rom);

    /**
     * Start cpu
     * Only call after boot rom was set
     */
    void run();

    /**
     * Execute single step. Returns false if opcode at current program counter is invalid, else true
     */
    bool step();

    void print_registers() const;

private:
    /**
     * Read memory value from address.
     */
    [[nodiscard]] uint8_t read_memory(u_int16_t address) const;
    [[nodiscard]] uint8_t& read_memory(u_int16_t address);
    /**
     * Write value to ram at address
     */
    void write_memory(uint16_t address, uint8_t value);

    /**
     * Sets bit flag in flag register f
     */
    void set_zero_flag(BitValues value);
    void set_subtract_flag(BitValues value);
    void set_half_carry_flag(BitValues value);
    void set_carry_flag(BitValues value);

    /**
     * Common function for all XOR instructions.
     * XORs input against value of a and stores result in a. Sets zero flag if a is zero after XOR.
     */
    void xor8(uint8_t input);

    /**
     * Common function for all 16 byte loads (LD).
     * Is passed the register into which to load the immediate value.
     */
    void ld16(uint16_t& input);

    void ldd_hl();

    /**
     * Common function for all 2 byte increment instructions.
     */
    void inc16(uint16_t& input);
    /**
     * Common function for all 1 byte increment instructions.
     */
    void inc8(uint8_t& input);
};
