#include "opcodes.hpp"

namespace opcodes {

constexpr std::array<Instruction, 0x100> instructions{
    // 0x00
    Instruction{InstructionType::NOP},
    // 0x01 Load 16 bit immediate into BC
    Instruction{InstructionType::LD, InteractionType::ImmediateWord, RegisterType::BC},
    // 0x02 Save A to address pointed by BC
    Instruction{InstructionType::LD, InteractionType::AddressRegister_Register, RegisterType::BC,
                RegisterType::A},
    // 0x03
    Instruction{InstructionType::INC, InteractionType::Register, RegisterType::BC},
    // 0x04
    Instruction{InstructionType::INC, InteractionType::Register, RegisterType::B},
    // 0x05
    Instruction{InstructionType::DEC, InteractionType::Register, RegisterType::B},
    // 0x06 Load immediate byte into B
    Instruction{InstructionType::LD, InteractionType::ImmediateByte, RegisterType::B},
    // 0x07 Rotate A left with carry
    Instruction{InstructionType::RLC, InteractionType::Register, RegisterType::A},
    // 0x08 Save SP to address given by immediate word
    Instruction{InstructionType::LD, InteractionType::AddressWord_Register, RegisterType::None,
                RegisterType::SP},
    // 0x09 Add BC to HL
    Instruction{InstructionType::ADD, InteractionType::Register_Register, RegisterType::HL,
                RegisterType::BC},
    // 0x0A Load A from address pointed to by BC
    Instruction{InstructionType::LD, InteractionType::Register_AddressRegister, RegisterType::A,
                RegisterType::BC},
    // 0x0B
    Instruction{InstructionType::DEC, InteractionType::Register, RegisterType::BC},
    // 0x0C
    Instruction{InstructionType::INC, InteractionType::Register, RegisterType::C},
    // 0x0D
    Instruction{InstructionType::DEC, InteractionType::Register, RegisterType::C},
    // 0x0E
    Instruction{InstructionType::LD, InteractionType::ImmediateByte, RegisterType::C},
    // 0x0F
    Instruction{InstructionType::RRC, InteractionType::Register, RegisterType::A},
    // 0x10 STOP
    Instruction{InstructionType::STOP},
    // 0x11
    Instruction{InstructionType::LD, InteractionType::ImmediateWord, RegisterType::DE},
    // 0x12
    Instruction{InstructionType::LD, InteractionType::AddressRegister_Register, RegisterType::DE,
                RegisterType::A},
    // 0x13
    Instruction{InstructionType::INC, InteractionType::Register, RegisterType::DE},
    // 0x14
    Instruction{InstructionType::INC, InteractionType::Register, RegisterType::D},
    // 0x15
    Instruction{InstructionType::DEC, InteractionType::Register, RegisterType::D},
    // 0x16
    Instruction{InstructionType::LD, InteractionType::ImmediateByte, RegisterType::D},
    // 0x17
    Instruction{InstructionType::RL, InteractionType::Register, RegisterType::A},
    // 0x18
    Instruction{InstructionType::JR, InteractionType::ImmediateByte},
    // 0x19
    Instruction{InstructionType::ADD, InteractionType::Register_Register, RegisterType::HL,
                RegisterType::DE},
    // 0x1A
    Instruction{InstructionType::LD, InteractionType::Register_AddressRegister, RegisterType::A,
                RegisterType::DE},
    // 0x1B
    Instruction{InstructionType::DEC, InteractionType::Register, RegisterType::DE},
    // 0x1C
    Instruction{InstructionType::INC, InteractionType::Register, RegisterType::E},
    // 0x1D
    Instruction{InstructionType::DEC, InteractionType::Register, RegisterType::E},
    // 0x1E
    Instruction{InstructionType::LD, InteractionType::ImmediateByte, RegisterType::E},
    // 0x1F
    Instruction{InstructionType::RR, InteractionType::Register, RegisterType::A},
    // 0x20 Jump if last result was non zero
    Instruction{InstructionType::JR, InteractionType::ImmediateByte, RegisterType::None,
                RegisterType::None, ConditionType::NonZero},
    // 0x21
    Instruction{InstructionType::LD, InteractionType::ImmediateWord, RegisterType::HL},
    // 0x22
    Instruction{InstructionType::LDI, InteractionType::AddressRegister_Register, RegisterType::HL,
                RegisterType::A},
    // 0x23
    Instruction{InstructionType::INC, InteractionType::Register, RegisterType::HL},
    // 0x24
    Instruction{InstructionType::INC, InteractionType::Register, RegisterType::H},
    // 0x25
    Instruction{InstructionType::DEC, InteractionType::Register, RegisterType::H},
    // 0x26
    Instruction{InstructionType::LD, InteractionType::ImmediateByte, RegisterType::H},
    // 0x27 This instruction only exists for register A, so we don't have to specify the register
    // here
    Instruction{InstructionType::DAA},
    // 0x28 Jump if last result was zero
    Instruction{InstructionType::JR, InteractionType::ImmediateByte, RegisterType::None,
                RegisterType::None, ConditionType::Zero},
    // 0x29
    Instruction{InstructionType::ADD, InteractionType::Register_Register, RegisterType::HL,
                RegisterType::HL},
    // 0x2A
    Instruction{InstructionType::LDI, InteractionType::Register_AddressRegister, RegisterType::A,
                RegisterType::HL},
    // 0x2B
    Instruction{InstructionType::DEC, InteractionType::Register, RegisterType::HL},
    // 0x2C
    Instruction{InstructionType::INC, InteractionType::Register, RegisterType::L},
    // 0x2D
    Instruction{InstructionType::DEC, InteractionType::Register, RegisterType::L},
    // 0x2E
    Instruction{InstructionType::LD, InteractionType::ImmediateByte, RegisterType::L},
    // 0x2F This instruction only exists for register A, so we don't have to specify the register
    // here
    Instruction{InstructionType::CPL},
    // 0x30
    Instruction{InstructionType::JR, InteractionType::ImmediateByte, RegisterType::None,
                RegisterType::None, ConditionType::NonCarry},
    // 0x31
    Instruction{InstructionType::LD, InteractionType::ImmediateWord, RegisterType::SP},
    // 0x32 Save A to address pointed at by HL and decrement HL
    Instruction{InstructionType::LDD, InteractionType::AddressRegister_Register, RegisterType::HL,
                RegisterType::A},
    // 0x33
    Instruction{InstructionType::INC, InteractionType::Register, RegisterType::SP},
    // 0x34
    Instruction{InstructionType::INC, InteractionType::AddressRegister, RegisterType::HL},
    // 0x35
    Instruction{InstructionType::DEC, InteractionType::AddressRegister, RegisterType::HL},
    // 0x36
    Instruction{InstructionType::LD, InteractionType::AddressRegister_ImmediateByte,
                RegisterType::HL},
    // 0x37
    Instruction{},
    // 0x38
    Instruction{InstructionType::JR, InteractionType::ImmediateByte, RegisterType::None,
                RegisterType::None, ConditionType::Carry},
    // 0x39
    Instruction{},
    // 0x3A
    Instruction{InstructionType::LDD, InteractionType::Register_AddressRegister, RegisterType::A,
                RegisterType::HL},
    // 0x3B
    Instruction{InstructionType::DEC, InteractionType::Register, RegisterType::SP},
    // 0x3C
    Instruction{InstructionType::INC, InteractionType::Register, RegisterType::A},
    // 0x3D
    Instruction{InstructionType::DEC, InteractionType::Register, RegisterType::A},
    // 0x3E
    Instruction{InstructionType::LD, InteractionType::ImmediateByte, RegisterType::A},
    // 0x3F
    Instruction{},
    // 0x40
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::B,
                RegisterType::B},
    // 0x41
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::B,
                RegisterType::C},
    // 0x42
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::B,
                RegisterType::D},
    // 0x43
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::B,
                RegisterType::E},
    // 0x44
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::B,
                RegisterType::H},
    // 0x45
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::B,
                RegisterType::L},
    // 0x46
    Instruction{InstructionType::LD, InteractionType::Register_AddressRegister, RegisterType::B,
                RegisterType::HL},
    // 0x47
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::B,
                RegisterType::A},
    // 0x48
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::C,
                RegisterType::B},
    // 0x49
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::C,
                RegisterType::C},
    // 0x4A
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::C,
                RegisterType::D},
    // 0x4B
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::C,
                RegisterType::E},
    // 0x4C
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::C,
                RegisterType::H},
    // 0x4D
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::C,
                RegisterType::L},
    // 0x4E
    Instruction{InstructionType::LD, InteractionType::Register_AddressRegister, RegisterType::C,
                RegisterType::HL},
    // 0x4F
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::C,
                RegisterType::A},
    // 0x50
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::D,
                RegisterType::B},
    // 0x51
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::D,
                RegisterType::C},
    // 0x52
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::D,
                RegisterType::D},
    // 0x53
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::D,
                RegisterType::E},
    // 0x54
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::D,
                RegisterType::H},
    // 0x55
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::D,
                RegisterType::L},
    // 0x56
    Instruction{InstructionType::LD, InteractionType::Register_AddressRegister, RegisterType::D,
                RegisterType::HL},
    // 0x57
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::D,
                RegisterType::A},
    // 0x58
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::E,
                RegisterType::B},
    // 0x59
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::E,
                RegisterType::C},
    // 0x5A
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::E,
                RegisterType::D},
    // 0x5B
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::E,
                RegisterType::E},
    // 0x5C
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::E,
                RegisterType::H},
    // 0x5D
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::E,
                RegisterType::L},
    // 0x5E
    Instruction{InstructionType::LD, InteractionType::Register_AddressRegister, RegisterType::E,
                RegisterType::HL},
    // 0x5F
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::E,
                RegisterType::A},
    // 0x60
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::H,
                RegisterType::B},
    // 0x61
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::H,
                RegisterType::C},
    // 0x62
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::H,
                RegisterType::D},
    // 0x63
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::H,
                RegisterType::E},
    // 0x64
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::H,
                RegisterType::H},
    // 0x65
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::H,
                RegisterType::L},
    // 0x66
    Instruction{InstructionType::LD, InteractionType::Register_AddressRegister, RegisterType::H,
                RegisterType::HL},
    // 0x67
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::H,
                RegisterType::A},
    // 0x68
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::L,
                RegisterType::B},
    // 0x69
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::L,
                RegisterType::C},
    // 0x6A
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::L,
                RegisterType::D},
    // 0x6B
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::L,
                RegisterType::E},
    // 0x6C
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::L,
                RegisterType::H},
    // 0x6D
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::L,
                RegisterType::L},
    // 0x6E
    Instruction{InstructionType::LD, InteractionType::Register_AddressRegister, RegisterType::L,
                RegisterType::HL},
    // 0x6F
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::L,
                RegisterType::A},
    // 0x70
    Instruction{InstructionType::LD, InteractionType::AddressRegister_Register, RegisterType::HL,
                RegisterType::B},
    // 0x71
    Instruction{InstructionType::LD, InteractionType::AddressRegister_Register, RegisterType::HL,
                RegisterType::C},
    // 0x72
    Instruction{InstructionType::LD, InteractionType::AddressRegister_Register, RegisterType::HL,
                RegisterType::D},
    // 0x73
    Instruction{InstructionType::LD, InteractionType::AddressRegister_Register, RegisterType::HL,
                RegisterType::E},
    // 0x74
    Instruction{InstructionType::LD, InteractionType::AddressRegister_Register, RegisterType::HL,
                RegisterType::H},
    // 0x75
    Instruction{InstructionType::LD, InteractionType::AddressRegister_Register, RegisterType::HL,
                RegisterType::L},
    // 0x76
    Instruction{},
    // 0x77
    Instruction{InstructionType::LD, InteractionType::AddressRegister_Register, RegisterType::HL,
                RegisterType::A},
    // 0x78
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::A,
                RegisterType::B},
    // 0x79
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::A,
                RegisterType::C},
    // 0x7A
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::A,
                RegisterType::D},
    // 0x7B
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::A,
                RegisterType::E},
    // 0x7C
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::A,
                RegisterType::H},
    // 0x7D
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::A,
                RegisterType::L},
    // 0x7E
    Instruction{InstructionType::LD, InteractionType::Register_AddressRegister, RegisterType::A,
                RegisterType::HL},
    // 0x7F
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::A,
                RegisterType::A},
    // 0x80
    Instruction{InstructionType::ADD, InteractionType::Register_Register, RegisterType::A,
                RegisterType::B},
    // 0x81
    Instruction{InstructionType::ADD, InteractionType::Register_Register, RegisterType::A,
                RegisterType::C},
    // 0x82
    Instruction{InstructionType::ADD, InteractionType::Register_Register, RegisterType::A,
                RegisterType::D},
    // 0x83
    Instruction{InstructionType::ADD, InteractionType::Register_Register, RegisterType::A,
                RegisterType::E},
    // 0x84
    Instruction{InstructionType::ADD, InteractionType::Register_Register, RegisterType::A,
                RegisterType::H},
    // 0x85
    Instruction{InstructionType::ADD, InteractionType::Register_Register, RegisterType::A,
                RegisterType::L},
    // 0x86
    Instruction{InstructionType::ADD, InteractionType::Register_AddressRegister, RegisterType::A,
                RegisterType::HL},
    // 0x87
    Instruction{InstructionType::ADD, InteractionType::Register_Register, RegisterType::A,
                RegisterType::A},
    // 0x88
    Instruction{},
    // 0x89
    Instruction{},
    // 0x8A
    Instruction{},
    // 0x8B
    Instruction{},
    // 0x8C
    Instruction{},
    // 0x8D
    Instruction{},
    // 0x8E
    Instruction{},
    // 0x8F
    Instruction{},
    // 0x90
    Instruction{InstructionType::SUB, InteractionType::Register_Register, RegisterType::A,
                RegisterType::B},
    // 0x91
    Instruction{InstructionType::SUB, InteractionType::Register_Register, RegisterType::A,
                RegisterType::C},
    // 0x92
    Instruction{InstructionType::SUB, InteractionType::Register_Register, RegisterType::A,
                RegisterType::D},
    // 0x93
    Instruction{InstructionType::SUB, InteractionType::Register_Register, RegisterType::A,
                RegisterType::E},
    // 0x94
    Instruction{InstructionType::SUB, InteractionType::Register_Register, RegisterType::A,
                RegisterType::H},
    // 0x95
    Instruction{InstructionType::SUB, InteractionType::Register_Register, RegisterType::A,
                RegisterType::L},
    // 0x96
    Instruction{InstructionType::SUB, InteractionType::Register_AddressRegister, RegisterType::A,
                RegisterType::HL},
    // 0x97
    Instruction{InstructionType::SUB, InteractionType::Register_Register, RegisterType::A,
                RegisterType::A},
    // 0x98
    Instruction{},
    // 0x99
    Instruction{},
    // 0x9A
    Instruction{},
    // 0x9B
    Instruction{},
    // 0x9C
    Instruction{},
    // 0x9D
    Instruction{},
    // 0x9E
    Instruction{},
    // 0x9F
    Instruction{},
    // 0xA0
    Instruction{},
    // 0xA1
    Instruction{},
    // 0xA2
    Instruction{},
    // 0xA3
    Instruction{},
    // 0xA4
    Instruction{},
    // 0xA5
    Instruction{},
    // 0xA6
    Instruction{},
    // 0xA7
    Instruction{},
    // 0xA8 A = A XOR B
    Instruction{InstructionType::XOR, InteractionType::Register_Register, RegisterType::A,
                RegisterType::B},
    // 0xA9
    Instruction{InstructionType::XOR, InteractionType::Register_Register, RegisterType::A,
                RegisterType::C},
    // 0xAA
    Instruction{InstructionType::XOR, InteractionType::Register_Register, RegisterType::A,
                RegisterType::D},
    // 0xAB
    Instruction{InstructionType::XOR, InteractionType::Register_Register, RegisterType::A,
                RegisterType::E},
    // 0xAC
    Instruction{InstructionType::XOR, InteractionType::Register_Register, RegisterType::A,
                RegisterType::H},
    // 0xAD
    Instruction{InstructionType::XOR, InteractionType::Register_Register, RegisterType::A,
                RegisterType::L},
    // 0xAE
    Instruction{},
    // 0xAF
    Instruction{InstructionType::XOR, InteractionType::Register_Register, RegisterType::A,
                RegisterType::A},
    // 0xB0
    Instruction{},
    // 0xB1
    Instruction{},
    // 0xB2
    Instruction{},
    // 0xB3
    Instruction{},
    // 0xB4
    Instruction{},
    // 0xB5
    Instruction{},
    // 0xB6
    Instruction{},
    // 0xB7
    Instruction{},
    // 0xB8
    Instruction{InstructionType::CP, InteractionType::Register_Register, RegisterType::A,
                RegisterType::B},
    // 0xB9
    Instruction{InstructionType::CP, InteractionType::Register_Register, RegisterType::A,
                RegisterType::C},
    // 0xBA
    Instruction{InstructionType::CP, InteractionType::Register_Register, RegisterType::A,
                RegisterType::D},
    // 0xBB
    Instruction{InstructionType::CP, InteractionType::Register_Register, RegisterType::A,
                RegisterType::E},
    // 0xBC
    Instruction{InstructionType::CP, InteractionType::Register_Register, RegisterType::A,
                RegisterType::H},
    // 0xBD
    Instruction{InstructionType::CP, InteractionType::Register_Register, RegisterType::A,
                RegisterType::L},
    // 0xBE
    Instruction{InstructionType::CP, InteractionType::Register_AddressRegister, RegisterType::A,
                RegisterType::HL},
    // 0xBF
    Instruction{InstructionType::CP, InteractionType::Register_Register, RegisterType::A,
                RegisterType::A},
    // 0xC0
    Instruction{InstructionType::RET, InteractionType::None, RegisterType::None, RegisterType::None,
                ConditionType::NonZero},
    // 0xC1
    Instruction{InstructionType::POP, InteractionType::None, RegisterType::BC},
    // 0xC2
    Instruction{},
    // 0xC3
    Instruction{},
    // 0xC4
    Instruction{InstructionType::CALL, InteractionType::ImmediateWord, RegisterType::None,
                RegisterType::None, ConditionType::NonZero},
    // 0xC5
    Instruction{InstructionType::PUSH, InteractionType::None, RegisterType::BC},
    // 0xC6
    Instruction{InstructionType::ADD, InteractionType::ImmediateByte, RegisterType::A},
    // 0xC7
    Instruction{},
    // 0xC8
    Instruction{InstructionType::RET, InteractionType::None, RegisterType::None, RegisterType::None,
                ConditionType::Zero},
    // 0xC9
    Instruction{InstructionType::RET},
    // 0xCA
    Instruction{},
    // 0xCB
    Instruction{InstructionType::CB, InteractionType::ImmediateByte},
    // 0xCC
    Instruction{InstructionType::CALL, InteractionType::ImmediateWord, RegisterType::None,
                RegisterType::None, ConditionType::Zero},
    // 0xCD
    Instruction{InstructionType::CALL, InteractionType::ImmediateWord},
    // 0xCE
    Instruction{},
    // 0xCF
    Instruction{},
    // 0xD0
    Instruction{InstructionType::RET, InteractionType::None, RegisterType::None, RegisterType::None,
                ConditionType::NonCarry},
    // 0xD1
    Instruction{InstructionType::POP, InteractionType::None, RegisterType::DE},
    // 0xD2
    Instruction{},
    // 0xD3
    Instruction{},
    // 0xD4
    Instruction{InstructionType::CALL, InteractionType::ImmediateWord, RegisterType::None,
                RegisterType::None, ConditionType::NonCarry},
    // 0xD5
    Instruction{InstructionType::PUSH, InteractionType::None, RegisterType::DE},
    // 0xD6
    Instruction{InstructionType::SUB, InteractionType::ImmediateByte, RegisterType::A},
    // 0xD7
    Instruction{},
    // 0xD8
    Instruction{InstructionType::RET, InteractionType::None, RegisterType::None, RegisterType::None,
                ConditionType::Carry},
    // 0xD9
    Instruction{},
    // 0xDA
    Instruction{},
    // 0xDB
    Instruction{},
    // 0xDC
    Instruction{InstructionType::CALL, InteractionType::ImmediateWord, RegisterType::None,
                RegisterType::None, ConditionType::Carry},
    // 0xDD
    Instruction{},
    // 0xDE
    Instruction{},
    // 0xDF
    Instruction{},
    // 0xE0
    Instruction{InstructionType::LDH, InteractionType::ImmediateByte, RegisterType::None,
                RegisterType::A},
    // 0xE1
    Instruction{InstructionType::POP, InteractionType::None, RegisterType::HL},
    // 0xE2
    Instruction{InstructionType::LDH, InteractionType::None, RegisterType::C, RegisterType::A},
    // 0xE3
    Instruction{},
    // 0xE4
    Instruction{},
    // 0xE5
    Instruction{InstructionType::PUSH, InteractionType::None, RegisterType::HL},
    // 0xE6
    Instruction{},
    // 0xE7
    Instruction{},
    // 0xE8
    Instruction{},
    // 0xE9
    Instruction{},
    // 0xEA
    Instruction{InstructionType::LD, InteractionType::AddressWord_Register, RegisterType::None,
                RegisterType::A},
    // 0xEB
    Instruction{},
    // 0xEC
    Instruction{},
    // 0xED
    Instruction{},
    // 0xEE
    Instruction{},
    // 0xEF
    Instruction{},
    // 0xF0
    Instruction{InstructionType::LDH, InteractionType::ImmediateByte, RegisterType::A},
    // 0xF1
    Instruction{InstructionType::POP, InteractionType::None, RegisterType::AF},
    // 0xF2
    Instruction{},
    // 0xF3
    Instruction{},
    // 0xF4
    Instruction{},
    // 0xF5
    Instruction{InstructionType::PUSH, InteractionType::None, RegisterType::AF},
    // 0xF6
    Instruction{},
    // 0xF7
    Instruction{},
    // 0xF8
    Instruction{InstructionType::LDHL, InteractionType::ImmediateByte, RegisterType::HL,
                RegisterType::SP},
    // 0xF9
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::SP,
                RegisterType::HL},
    // 0xFA
    Instruction{InstructionType::LD, InteractionType::Register_AddressWord, RegisterType::A},
    // 0xFB
    Instruction{},
    // 0xFC
    Instruction{},
    // 0xFD
    Instruction{},
    // 0xFE
    Instruction{InstructionType::CP, InteractionType::ImmediateByte, RegisterType::A},
    // 0xFF
    Instruction{}};

}

opcodes::Instruction opcodes::get_instruction_by_value(uint8_t value) {
    return opcodes::instructions[value];
}
