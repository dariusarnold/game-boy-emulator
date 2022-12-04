#pragma once

#include "bitmanipulation.hpp"
#include "constants.h"
#include "opcodes.hpp"
#include "registers.hpp"

#include "fmt/format.h"

#include <memory>
#include <algorithm>
#include <array>
#include <cstdint>

namespace spdlog {
class logger;
}
class Emulator;


// Typedef for clock cycles.
// Gives a more descriptive name but doesn't change semantics.
using t_cycle = size_t;


class Cpu {
    Registers registers = {};
    Emulator* m_emulator;
    std::shared_ptr<spdlog::logger> m_logger;

    opcodes::Instruction current_instruction;
    opcodes::Instruction previous_instruction;

public:
    explicit Cpu(Emulator* emulator);

    /**
     * Start cpu.
     * Only call after boot rom was set.
     */
    void run();

    /**
     * Execute single step. Returns false if opcode at current program counter is invalid, else true
     */
    bool step();

    // Set values of registers as if boot rom was run
    void set_initial_state();

    std::string get_minimal_debug_state();

    opcodes::Instruction get_current_instruction();
    opcodes::Instruction get_previous_instruction();

    void call_isr(uint16_t isr_address);

private:
    template <typename T>
    [[noreturn]] void abort_execution(std::string_view msg) {
        auto complete_msg = fmt::format("CPU ERROR: {}", msg);
        throw T{complete_msg};
    }
    /**
     * Sets bit flag in flag register f
     */
    void set_zero_flag(BitValues value);
    void set_subtract_flag(BitValues value);
    void set_half_carry_flag(BitValues value);
    void set_carry_flag(BitValues value);
    void set_zero_flag(bool flag_set);
    void set_subtract_flag(bool flag_set);
    void set_half_carry_flag(bool flag_set);
    void set_carry_flag(bool flag_set);

    /**
     * Return true if flag is set, otherwise return false.
     */
    [[nodiscard]] bool is_flag_set(flags flag) const;

    /**
     * Common function for all bit test instructions.
     * Test if bit at position in value is set and sets zero flag accordingly.
     * @param value
     * @param position
     */
    void instruction_cb_test_bit(opcodes::RegisterType register_type, uint8_t position);

    /**
     * Convert second byte of cb opcodes to the register, upon which this instruction operators
     * @param opcode_byte
     * @return
     */
    uint8_t op_code_to_register(uint8_t opcode_byte);

    /**
     * Write value to register specified by the opcode. Used for the second byte of CB instruction.
     */
    void write_to_destination(uint8_t opcode_byte, uint8_t value);

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

    [[nodiscard]] bool check_condition(opcodes::ConditionType condition_type) const;

    // Helper which reads data for CB instructions either from register or main memory in case of
    // indirect access.
    uint8_t cb_fetch_data(opcodes::RegisterType register_type);

    // Helper which writes data for CB instructions. Write either to destination register or main
    // memory in case of indirect access.
    void cb_set_data(opcodes::RegisterType register_type, uint8_t value);

    void instruction_cb_rotate_left(opcodes::RegisterType register_type);

    void instruction_cb_set_reset_bit(opcodes::InstructionType instruction_type,
                                      opcodes::RegisterType register_type, uint8_t bit);

    void instruction_cb_srl(opcodes::RegisterType register_type);

    void instructionLD(opcodes::Instruction instruction, uint16_t data);
    void instructionLDH(opcodes::Instruction instruction, uint16_t data);

    // data is the second byte of the whole CB instruction
    void instructionCB(uint8_t cb_opcode);

    void instructionJR(opcodes::Instruction instruction, uint8_t data);

    void instructionINCDEC(opcodes::Instruction instruction);

    void instructionCALL(opcodes::Instruction instruction, uint16_t data);

    void instructionPUSH(opcodes::Instruction instruction);

    void instructionPOP(opcodes::Instruction instruction);

    void instructionRL(opcodes::Instruction instruction);

    void instructionRET(opcodes::Instruction instruction);

    void instructionCP(opcodes::Instruction instruction, uint8_t data);

    void instructionANDORXOR(opcodes::Instruction instruction, uint8_t data);

    void instructionSUB(opcodes::Instruction instruction, uint8_t data);

    void instructionADD(opcodes::Instruction instruction, uint8_t data);

    void instructionJP(opcodes::Instruction instruction, uint16_t data);

    void instructionRETI();
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
