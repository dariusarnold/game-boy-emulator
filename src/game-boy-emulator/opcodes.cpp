#include "opcodes.hpp"

opcodes::OpCodeInfo LD_SP_NN{0x31, "LD SP NN", 12};
opcodes::OpCodeInfo XOR_A{0xAF, "XOR A", 4};
opcodes::OpCodeInfo XOR_B{0xA8, "XOR B", 4};
opcodes::OpCodeInfo XOR_C{0xA9, "XOR C", 4};
opcodes::OpCodeInfo XOR_D{0xAA, "XOR D", 4};
opcodes::OpCodeInfo XOR_E{0xAB, "XOR E", 4};
opcodes::OpCodeInfo XOR_H{0xAC, "XOR H", 4};
opcodes::OpCodeInfo XOR_L{0xAD, "XOR L", 4};
opcodes::OpCodeInfo XOR_HL{0xAE, "XOR HL", 8};
opcodes::OpCodeInfo XOR_N{0xEE, "XOR N", 8};