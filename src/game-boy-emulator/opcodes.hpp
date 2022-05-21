#pragma once

#include <cstdint>
#include <string>
#include <fmt/format.h>


namespace opcodes {

    struct OpCode {
        uint8_t value;
        bool extendend = false;
    };

    // Comparison operator for std::unordered_map
    inline bool operator==(const OpCode& op1, const OpCode& op2) {
        return op1.value == op2.value && op1.extendend == op2.extendend;
    }

    constexpr OpCode NOP{0x00};
    // 2 byte opcode for extended instructions
    constexpr OpCode CB{0xCB};
    // Test bit, set zero flag
    constexpr OpCode BIT_0B{0x40};
    constexpr OpCode BIT_7A{0x7F};
    // Reset 1 bit in register
    constexpr OpCode RES_0B{0x80};
    constexpr OpCode RES_7A{0xAF};
    // Set 1 bit in register
    constexpr OpCode SET_0B{0xC0};
    constexpr OpCode SET_7A{0xFF};
    // Jump if condition after adding signed 8 bit immediate value to address
    constexpr OpCode JR_NZ{0x20};
    constexpr OpCode JR_Z{0x28};
    constexpr OpCode JR_NC{0x30};
    constexpr OpCode JR_C{0x38};
    // Jump by signed 8 bit immediate value
    constexpr OpCode JR{0x18};
    // Increment 1 byte register
    constexpr OpCode INC_A{0x3C};
    constexpr OpCode INC_B{0x04};
    constexpr OpCode INC_C{0x0C};
    constexpr OpCode INC_D{0x14};
    constexpr OpCode INC_E{0x1C};
    constexpr OpCode INC_H{0x24};
    constexpr OpCode INC_L{0x2C};
    // Increment value pointed at by HL
    constexpr OpCode INC_HL_INDIRECT{0x34};
    // Increment 2 byte register
    constexpr OpCode INC_BC{0x03};
    constexpr OpCode INC_DE{0x13};
    constexpr OpCode INC_HL{0x23};
    constexpr OpCode INC_SP{0x33};
    // Decrement 1 byte register
    constexpr OpCode DEC_A{0x3D};
    constexpr OpCode DEC_B{0x05};
    constexpr OpCode DEC_C{0x0D};
    constexpr OpCode DEC_D{0x15};
    constexpr OpCode DEC_E{0x1D};
    constexpr OpCode DEC_H{0x25};
    constexpr OpCode DEC_L{0x2D};
    // Load 2 byte immediate to register
    constexpr OpCode LD_BC_NN{0x01};
    constexpr OpCode LD_DE_NN{0x11};
    constexpr OpCode LD_HL_NN{0x21};
    constexpr OpCode LD_SP_NN{0x31};
    // Load 1 byte immediate to register
    constexpr OpCode LD_C_N{0x0E};
    constexpr OpCode LD_E_N{0x1E};
    constexpr OpCode LD_L_N{0x2E};
    constexpr OpCode LD_A_N{0x3E};
    constexpr OpCode LD_B_N{0x06};
    constexpr OpCode LD_D_N{0x16};
    constexpr OpCode LD_H_N{0x26};
    // 1 byte xor, store in A
    constexpr OpCode XOR_A{0xAF};
    constexpr OpCode XOR_B{0xA8};
    constexpr OpCode XOR_C{0xA9};
    constexpr OpCode XOR_D{0xAA};
    constexpr OpCode XOR_E{0xAB};
    constexpr OpCode XOR_H{0xAC};
    constexpr OpCode XOR_L{0xAD};
    constexpr OpCode XOR_HL{0xAE};
    constexpr OpCode XOR_N{0xEE};
    // Save A to address pointed at by C + 0xFF00
    constexpr OpCode LDH_C_A{0xE2};
    // Save A to address pointed at by 8 bit immediate + 0xFF00
    constexpr OpCode LDH_N_A{0xE0};
    // Save a to address pointed at by BC
    constexpr OpCode LD_A_BC{0x0A};
    // Save a to address pointed at by DE
    constexpr OpCode LD_A_DE{0x1A};
    // Save to address pointed at by register HL
    constexpr OpCode LD_HL_B{0x70};
    constexpr OpCode LD_HL_C{0x71};
    constexpr OpCode LD_HL_D{0x72};
    constexpr OpCode LD_HL_E{0x73};
    constexpr OpCode LD_HL_H{0x74};
    constexpr OpCode LD_HL_L{0x75};
    constexpr OpCode LD_HL_A{0x77};
    // Call function at 16 bit location
    constexpr OpCode CALL_NN{0xCD};
    // Copy register to register
    constexpr OpCode LD_B_B{0x40};
    constexpr OpCode LD_B_C{0x41};
    constexpr OpCode LD_B_D{0x42};
    constexpr OpCode LD_B_E{0x43};
    constexpr OpCode LD_B_H{0x44};
    constexpr OpCode LD_B_L{0x45};
    constexpr OpCode LD_B_A{0x47};
    constexpr OpCode LD_C_B{0x48};
    constexpr OpCode LD_C_C{0x49};
    constexpr OpCode LD_C_D{0x4A};
    constexpr OpCode LD_C_E{0x4B};
    constexpr OpCode LD_C_H{0x4C};
    constexpr OpCode LD_C_L{0x4D};
    constexpr OpCode LD_C_A{0x4F};
    constexpr OpCode LD_D_B{0x50};
    constexpr OpCode LD_D_C{0x51};
    constexpr OpCode LD_D_D{0x52};
    constexpr OpCode LD_D_E{0x53};
    constexpr OpCode LD_D_H{0x54};
    constexpr OpCode LD_D_L{0x55};
    constexpr OpCode LD_D_A{0x57};
    constexpr OpCode LD_E_B{0x58};
    constexpr OpCode LD_E_C{0x59};
    constexpr OpCode LD_E_D{0x5A};
    constexpr OpCode LD_E_E{0x5B};
    constexpr OpCode LD_E_H{0x5C};
    constexpr OpCode LD_E_L{0x5D};
    constexpr OpCode LD_E_A{0x5F};
    constexpr OpCode LD_H_B{0x60};
    constexpr OpCode LD_H_C{0x61};
    constexpr OpCode LD_H_D{0x62};
    constexpr OpCode LD_H_E{0x63};
    constexpr OpCode LD_H_H{0x64};
    constexpr OpCode LD_H_L{0x65};
    constexpr OpCode LD_H_A{0x67};
    constexpr OpCode LD_L_B{0x68};
    constexpr OpCode LD_L_C{0x69};
    constexpr OpCode LD_L_D{0x6A};
    constexpr OpCode LD_L_E{0x6B};
    constexpr OpCode LD_L_H{0x6C};
    constexpr OpCode LD_L_L{0x6D};
    constexpr OpCode LD_L_A{0x6F};
    constexpr OpCode LD_A_B{0x78};
    constexpr OpCode LD_A_C{0x79};
    constexpr OpCode LD_A_D{0x7A};
    constexpr OpCode LD_A_E{0x7B};
    constexpr OpCode LD_A_H{0x7C};
    constexpr OpCode LD_A_L{0x7D};
    constexpr OpCode LD_A_A{0x7F};
    // Push 2 byte register onto stack
    constexpr OpCode PUSH_BC{0xC5};
    constexpr OpCode PUSH_DE{0xD5};
    constexpr OpCode PUSH_HL{0xE5};
    constexpr OpCode PUSH_AF{0xF5};
    // Rotate value in register left
    constexpr OpCode RL_A{0x17};
    constexpr OpCode RL_B{0x10};
    constexpr OpCode RL_C{0x11};
    constexpr OpCode RL_D{0x12};
    constexpr OpCode RL_E{0x13};
    constexpr OpCode RL_H{0x14};
    constexpr OpCode RL_L{0x15};
    // Save A to address pointed by HL and increment HL
    constexpr OpCode LDI_HL_A{0x22};
    // Load A from address pointed by HL and increment HL
    constexpr OpCode LDI_A_HL{0x2A};
    // Save A to address pointed by HL and decrement HL
    constexpr OpCode LDD_HL_A{0x32};
    // Load A from address pointed by HL and decrement HL
    constexpr OpCode LDD_A_HL{0x3A};
    // Unconditional return
    constexpr OpCode RET{0xC9};
    // Pop 16bit value from stack into register
    constexpr OpCode POP_BC{0xC1};
    constexpr OpCode POP_DE{0xD1};
    constexpr OpCode POP_HL{0xE1};
    constexpr OpCode POP_AF{0xF1};

} // namespace opcodes

// hash specialization for std::unordered_map
namespace std {
    template <>
    struct hash<opcodes::OpCode> {
        size_t operator()(const opcodes::OpCode& op) const {
            return std::hash<uint8_t>{}(op.value);
        }
    };
} // namespace std


template <>
struct fmt::formatter<opcodes::OpCode> {

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
    auto format(const opcodes::OpCode& opcode, FormatContext& context) {
        if (opcode.extendend) {
            return format_to(context.out(), "0xCB {:02X}", opcode.value);
        }
        return format_to(context.out(), "{:02X}", opcode.value);
    }
};