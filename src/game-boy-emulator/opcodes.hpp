#pragma once

#include <cstdint>
#include <string>

namespace opcodes {

    using OpCode = uint8_t;

    struct OpCodeInfo {
        uint8_t code;
        std::string mnemonic;
        // How long the instruction takes to execute
        uint8_t cycles;
    };

    // Comparison operator for std::unordered_map
    inline bool operator==(const OpCodeInfo& op1, const OpCodeInfo& op2) {
        return op1.code == op2.code;
    }

    constexpr OpCode NOP = 0x00;
    // Increment 1 byte register
    constexpr OpCode INC_A = 0x3C;
    constexpr OpCode INC_B = 0x04;
    constexpr OpCode INC_C = 0x0C;
    constexpr OpCode INC_D = 0x14;
    constexpr OpCode INC_E = 0x1C;
    constexpr OpCode INC_H = 0x24;
    constexpr OpCode INC_L = 0x2C;
    // Increment value pointed at by HL
    constexpr OpCode INC_HL_INDIRECT = 0x34;
    // Increment 2 byte register
    constexpr OpCode INC_BC = 0x03;
    constexpr OpCode INC_DE = 0x13;
    constexpr OpCode INC_HL = 0x23;
    constexpr OpCode INC_SP = 0x33;
    constexpr OpCode LD_BC_NN = 0x01;
    constexpr OpCode LD_DE_NN = 0x11;
    constexpr OpCode LD_HL_NN = 0x21;
    constexpr OpCode LD_SP_NN = 0x31;
    constexpr OpCode LDD_HL_A = 0x32;
    // 1 byte xor, store in A
    constexpr OpCode XOR_A = 0xAF;
    constexpr OpCode XOR_B = 0xA8;
    constexpr OpCode XOR_C = 0xA9;
    constexpr OpCode XOR_D = 0xAA;
    constexpr OpCode XOR_E = 0xAB;
    constexpr OpCode XOR_H = 0xAC;
    constexpr OpCode XOR_L = 0xAD;
    constexpr OpCode XOR_HL = 0xAE;
    constexpr OpCode XOR_N = 0xEE;

} // namespace opcodes

// hash specialization for std::unordered_map
namespace std {
    template <>
    struct hash<opcodes::OpCodeInfo> {
        size_t operator()(const opcodes::OpCodeInfo& op) const {
            return std::hash<uint8_t>{}(op.code);
        }
    };
} // namespace std
