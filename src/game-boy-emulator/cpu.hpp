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


struct CpuDebugState {
    uint8_t a;
    uint8_t f;
    uint8_t b;
    uint8_t c;
    uint8_t d;
    uint8_t e;
    uint8_t h;
    uint8_t l;
    uint16_t sp;
    uint16_t pc;
    std::array<uint8_t, 4> mem_pc;
    friend bool operator==(const CpuDebugState& a, const CpuDebugState& b);
};

std::ostream& operator<<(std::ostream& os, const CpuDebugState& cds);

class Cpu {
    Registers registers = {};
    Emulator* m_emulator;
    std::shared_ptr<spdlog::logger> m_logger;

    opcodes::Instruction current_instruction;
    opcodes::Instruction previous_instruction;
    // Amount of cycles executing the last instruction took.
    size_t m_cycles_previous_instruction = 0;

public:
    explicit Cpu(Emulator* emulator);

    /**
     * Start cpu.
     * Only call after boot rom was set.
     */
    [[noreturn]] void run();

    /**
     * Execute single step. Returns false if opcode at current program counter is invalid, else true
     */
    void step();

    // Set values of registers as if boot rom was run
    void set_initial_state();

    std::string get_minimal_debug_state();

    CpuDebugState get_debug_state();

    [[nodiscard]] opcodes::Instruction get_current_instruction() const;
    [[nodiscard]] opcodes::Instruction get_previous_instruction() const;
    [[nodiscard]] size_t cycle_duration_previous_instruction() const;

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
     * Read next instruction from address at program counter.
     * After this method reads the new opcode, the program counter will be incremented to point past
     * the opcode read
     * @return
     */
    opcodes::Instruction fetch_instruction();

    // Only instructions which actually fetch data return a value. All other instructions return an
    // empty optional.
    std::optional<uint16_t> fetch_data(opcodes::Instruction instruction);

    void set_register_value(opcodes::RegisterType register_type, uint16_t value);
    uint16_t get_register_value(opcodes::RegisterType register_type);

    // Helper which puts a value onto the stack in right endian order and elapsing two cycles.
    void push_word_on_stack(uint16_t x);
    // Helper which pops a value from the stack in the right endian order and elapsing two cycles.
    uint16_t pop_word_from_stack();

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

    void instruction_cb_rr(opcodes::RegisterType register_type);

    void instruction_cb_swap(opcodes::RegisterType register_type);

    void instruction_cb_rlc(opcodes::RegisterType register_type);
    void instruction_cb_rrc(opcodes::RegisterType register_type);

    void instruction_cb_sla(opcodes::RegisterType register_type);
    void instruction_cb_sra(opcodes::RegisterType register_type);

    void instructionLD(opcodes::Instruction instruction, uint16_t data);
    void instructionLDH(opcodes::Instruction instruction, uint16_t data);
    void instructionLDHL(uint8_t data);

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

    void instructionADD(opcodes::Instruction instruction, uint16_t data);

    void instructionADD_Signed(int8_t data);

    void instructionJP(opcodes::Instruction instruction, uint16_t data);

    void instructionRETI();

    void instructionADC(opcodes::Instruction instruction, uint8_t data);

    void instructionSBC(opcodes::Instruction instruction, uint8_t data);

    void instructionDAA();

    void instructionRST(uint8_t opcode);

    void instructionCPL();

    void instructionSCF();

    void instructionCCF();

    uint8_t instructionRLC(uint8_t data);

    uint8_t instructionRRC(uint8_t data);
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

inline bool was_half_carry_word(uint16_t a, uint16_t b) {
    return (a & 0xfff) + (b & 0xfff) > 0xfff;
}

template <typename F>
bool was_carry(uint8_t a, uint8_t b, const F& operation) {
    bool c = operation(static_cast<uint16_t>(a), static_cast<uint16_t>(b)) & 0x100;
    return c;
}

inline bool was_carry_word(uint16_t a, uint16_t b) {
    unsigned result = a + b;
    return (result & 0x10000) != 0;
}

} // namespace internal
