#pragma once

#include <cstdint>
#include <string>

namespace opcodes {

    struct OpCode {
        uint8_t value;
    };

    // Comparison operator for std::unordered_map
    inline bool operator==(const OpCode& op1, const OpCode& op2) {
        return op1.value == op2.value;
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
    constexpr OpCode LD_BC_NN{0x01};
    constexpr OpCode LD_DE_NN{0x11};
    constexpr OpCode LD_HL_NN{0x21};
    constexpr OpCode LD_SP_NN{0x31};
    constexpr OpCode LDD_HL_A{0x32};
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
