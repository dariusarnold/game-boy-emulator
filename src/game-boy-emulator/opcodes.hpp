#pragma once

#include "bitmanipulation.hpp"

#include <magic_enum.hpp>

#include <cstdint>
#include <fmt/format.h>
#include <string>


namespace opcodes {

enum class InstructionType {
    NONE,
    NOP,
    LD,
    // Load with address register increment
    LDI,
    // Load with address register decrement
    LDD,
    // Load with offset 0xFF00
    LDH,
    // Add signed immediate byte to SP and save in HL
    LDHL,
    ADD,
    SUB,
    INC,
    DEC,
    // Rotate left with carry
    RLC,
    // Rotate right with carry
    RRC,
    // Rotate right
    RR,
    // Rotate left
    RL,
    STOP,
    // Relative jump by signed immediate
    JR,
    // Adjust for BCD addition
    DAA,
    // Complement (Logical NOT)
    CPL,
    XOR,
    CB,
    CALL,
    RET,
    PUSH,
    POP,
    // Compare
    CP,
};

enum class RegisterType {
    None,
    A,
    B,
    C,
    D,
    E,
    H,
    L,
    F,
    AF,
    BC,
    DE,
    HL,
    SP,
    PC,
};

/**
 * For the load instructions there are the following options:
 *    | Destination                    | Source                  | Additional variants
 * ---------------------------------------------------------------------
 * 01 | Register                       | Immediate Byte          |
 * 02 | Register                       | Immediate Word          |
 * 03 | Register                       | Register                |
 * 04 | AddressRegister                | Register                | Increment/decrement
 * AddressRegister 05 | Register                       | AddressRegister         |
 * Increment/decrement AddressRegister 06 | AddressRegister                | ImmediateByte | 07 |
 * AddressWord                    | Register                | 08 | AddressByte + fixed Offset     |
 * RegisterA               | 09 | AddressRegister + fixed Offset | RegisterA               | 10 |
 * RegisterA                      | AddressByte + fixedOffset | 11 | RegisterHL | RegisterSP +
 * OffsetByte | 12 | RegisterA                      | AddressWord             |
 *
 * Increment/decrement are implemented as instruction types LDI/LDD.
 * Fixed offsets are implemented as instruction type LDH.
 * 11 is implemented as instruction type LDHL
 */
enum class InteractionType {
    None,
    // Operate on a single register
    Register,
    // Operate on value pointed at by address register
    AddressRegister,
    // 01 - Read immediate byte
    ImmediateByte,
    // 02 - Read immediate word
    ImmediateWord,
    // 03 - Operation between two registers where one is modified
    Register_Register,
    // 04 - Save register value to address pointed at by other register
    AddressRegister_Register,
    // 05 - Load register value from address pointed at by other register
    Register_AddressRegister,
    // 06 - Save immediate byte to address pointed at by register
    AddressRegister_ImmediateByte,
    // 07 - Save register to address given by immediate word
    AddressWord_Register,
    // 12 - Load register from address given by immediate word
    Register_AddressWord,
};

enum class ConditionType {
    None,
    NonZero,
    Zero,
    Carry,
    NonCarry,
};

struct Instruction {
    InstructionType instruction_type = InstructionType::NONE;
    InteractionType interaction_type = InteractionType::None;
    RegisterType register_type_destination = RegisterType::None;
    RegisterType register_type_source = RegisterType::None;
    ConditionType condition_type = ConditionType::None;
};

Instruction get_instruction_by_value(uint8_t value);

} // namespace opcodes

/**
 * Formatted output for Instructions
 */
template <>
class fmt::formatter<opcodes::Instruction> {

    std::string format_string;

public:
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
        const auto* it = std::find(context.begin(), context.end(), '}');
        if (it == context.end()) {
            throw fmt::format_error("Invalid format");
        }
        format_string.assign(context.begin(), it);

        return it;
    }

    template <typename FormatContext>
    auto format(const opcodes::Instruction& instruction, FormatContext& context) {
        namespace me = magic_enum;
        return format_to(context.out(), "Instruction {} {} {} {} {}",
                         me::enum_name(instruction.instruction_type),
                         me::enum_name(instruction.interaction_type),
                         me::enum_name(instruction.register_type_destination),
                         me::enum_name(instruction.register_type_source),
                         me::enum_name(instruction.condition_type));
    }
};
