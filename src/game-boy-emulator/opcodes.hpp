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
    PUSH,
    POP,
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
 * 04 | AddressRegister                | Register                | Increment/decrement AddressRegister
 * 05 | Register                       | AddressRegister         | Increment/decrement AddressRegister
 * 06 | AddressRegister                | ImmediateByte           |
 * 07 | AddressWord                    | Register                |
 * 08 | AddressByte + fixed Offset     | RegisterA               |
 * 09 | AddressRegister + fixed Offset | RegisterA               |
 * 10 | RegisterA                      | AddressByte + fixedOffset |
 * 11 | RegisterHL                     | RegisterSP + OffsetByte |
 * 12 | RegisterA                      | AddressWord             |
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
    Instruction{InstructionType::LD, InteractionType::AddressWord_Register, RegisterType::None, RegisterType::SP},
    // 0x09 Add BC to HL
    Instruction{InstructionType::ADD, InteractionType::Register_Register, RegisterType::HL, RegisterType::BC},
    // 0x0A Load A from address pointed to by BC
    Instruction{InstructionType::LD, InteractionType::Register_AddressRegister, RegisterType::A, RegisterType::BC},
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
    Instruction{InstructionType::LD, InteractionType::AddressRegister_Register, RegisterType::DE, RegisterType::A},
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
    Instruction{InstructionType::ADD, InteractionType::Register_Register, RegisterType::HL, RegisterType::DE},
    // 0x1A
    Instruction{InstructionType::LD, InteractionType::Register_AddressRegister, RegisterType::A, RegisterType::DE},
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
    Instruction{InstructionType::JR, InteractionType::ImmediateByte, RegisterType::None, RegisterType::None, ConditionType::NonZero},
    // 0x21
    Instruction{InstructionType::LD, InteractionType::ImmediateWord, RegisterType::HL},
    // 0x22
    Instruction{InstructionType::LDI, InteractionType::AddressRegister_Register, RegisterType::HL, RegisterType::A},
    // 0x23
    Instruction{InstructionType::INC, InteractionType::Register, RegisterType::HL},
    // 0x24
    Instruction{InstructionType::INC, InteractionType::Register, RegisterType::H},
    // 0x25
    Instruction{InstructionType::DEC, InteractionType::Register, RegisterType::H},
    // 0x26
    Instruction{InstructionType::LD, InteractionType::ImmediateByte, RegisterType::H},
    // 0x27 This instruction only exists for register A, so we don't have to specify the register here
    Instruction{InstructionType::DAA},
    // 0x28 Jump if last result was zero
    Instruction{InstructionType::JR, InteractionType::ImmediateByte, RegisterType::None, RegisterType::None, ConditionType::Zero},
    // 0x29
    Instruction{InstructionType::ADD, InteractionType::Register_Register, RegisterType::HL, RegisterType::HL},
    // 0x2A
    Instruction{InstructionType::LDI, InteractionType::Register_AddressRegister, RegisterType::A, RegisterType::HL},
    // 0x2B
    Instruction{InstructionType::DEC, InteractionType::Register, RegisterType::HL},
    // 0x2C
    Instruction{InstructionType::INC, InteractionType::Register, RegisterType::L},
    // 0x2D
    Instruction{InstructionType::DEC, InteractionType::Register, RegisterType::L},
    // 0x2E
    Instruction{InstructionType::LD, InteractionType::ImmediateByte, RegisterType::L},
    // 0x2F This instruction only exists for register A, so we don't have to specify the register here
    Instruction{InstructionType::CPL},
    // 0x30
    Instruction{InstructionType::JR, InteractionType::ImmediateByte, RegisterType::None, RegisterType::None, ConditionType::NonCarry},
    // 0x31
    Instruction{InstructionType::LD, InteractionType::ImmediateWord, RegisterType::SP},
    // 0x32 Save A to address pointed at by HL and decrement HL
    Instruction{InstructionType::LDD, InteractionType::AddressRegister_Register, RegisterType::HL, RegisterType::A},
    // 0x33
    Instruction{InstructionType::INC, InteractionType::Register, RegisterType::SP},
    // 0x34
    Instruction{InstructionType::INC, InteractionType::AddressRegister, RegisterType::HL},
    // 0x35
    Instruction{InstructionType::DEC, InteractionType::AddressRegister, RegisterType::HL},
    // 0x36
    Instruction{InstructionType::LD, InteractionType::AddressRegister_ImmediateByte, RegisterType::HL},
    // 0x37
    Instruction{},
    // 0x38
    Instruction{InstructionType::JR, InteractionType::ImmediateByte, RegisterType::None, RegisterType::None, ConditionType::Carry},
    // 0x39
    Instruction{},
    // 0x3A
    Instruction{InstructionType::LDD, InteractionType::Register_AddressRegister, RegisterType::A, RegisterType::HL},
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
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::B, RegisterType::B},
    // 0x41
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::B, RegisterType::C},
    // 0x42
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::B, RegisterType::D},
    // 0x43
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::B, RegisterType::E},
    // 0x44
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::B, RegisterType::H},
    // 0x45
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::B, RegisterType::L},
    // 0x46
    Instruction{InstructionType::LD, InteractionType::Register_AddressRegister, RegisterType::B, RegisterType::HL},
    // 0x47
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::B, RegisterType::A},
    // 0x48
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::C, RegisterType::B},
    // 0x49
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::C, RegisterType::C},
    // 0x4A
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::C, RegisterType::D},
    // 0x4B
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::C, RegisterType::E},
    // 0x4C
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::C, RegisterType::H},
    // 0x4D
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::C, RegisterType::L},
    // 0x4E
    Instruction{InstructionType::LD, InteractionType::Register_AddressRegister, RegisterType::C, RegisterType::HL},
    // 0x4F
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::C, RegisterType::A},
    // 0x50
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::D, RegisterType::B},
    // 0x51
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::D, RegisterType::C},
    // 0x52
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::D, RegisterType::D},
    // 0x53
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::D, RegisterType::E},
    // 0x54
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::D, RegisterType::H},
    // 0x55
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::D, RegisterType::L},
    // 0x56
    Instruction{InstructionType::LD, InteractionType::Register_AddressRegister, RegisterType::D, RegisterType::HL},
    // 0x57
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::D, RegisterType::A},
    // 0x58
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::E, RegisterType::B},
    // 0x59
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::E, RegisterType::C},
    // 0x5A
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::E, RegisterType::D},
    // 0x5B
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::E, RegisterType::E},
    // 0x5C
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::E, RegisterType::H},
    // 0x5D
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::E, RegisterType::L},
    // 0x5E
    Instruction{InstructionType::LD, InteractionType::Register_AddressRegister, RegisterType::E, RegisterType::HL},
    // 0x5F
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::E, RegisterType::A},
    // 0x60
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::H, RegisterType::B},
    // 0x61
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::H, RegisterType::C},
    // 0x62
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::H, RegisterType::D},
    // 0x63
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::H, RegisterType::E},
    // 0x64
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::H, RegisterType::H},
    // 0x65
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::H, RegisterType::L},
    // 0x66
    Instruction{InstructionType::LD, InteractionType::Register_AddressRegister, RegisterType::H, RegisterType::HL},
    // 0x67
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::H, RegisterType::A},
    // 0x68
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::L, RegisterType::B},
    // 0x69
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::L, RegisterType::C},
    // 0x6A
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::L, RegisterType::D},
    // 0x6B
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::L, RegisterType::E},
    // 0x6C
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::L, RegisterType::H},
    // 0x6D
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::L, RegisterType::L},
    // 0x6E
    Instruction{InstructionType::LD, InteractionType::Register_AddressRegister, RegisterType::L, RegisterType::HL},
    // 0x6F
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::L, RegisterType::A},
    // 0x70
    Instruction{InstructionType::LD, InteractionType::AddressRegister_Register, RegisterType::HL, RegisterType::B},
    // 0x71
    Instruction{InstructionType::LD, InteractionType::AddressRegister_Register, RegisterType::HL, RegisterType::C},
    // 0x72
    Instruction{InstructionType::LD, InteractionType::AddressRegister_Register, RegisterType::HL, RegisterType::D},
    // 0x73
    Instruction{InstructionType::LD, InteractionType::AddressRegister_Register, RegisterType::HL, RegisterType::E},
    // 0x74
    Instruction{InstructionType::LD, InteractionType::AddressRegister_Register, RegisterType::HL, RegisterType::H},
    // 0x75
    Instruction{InstructionType::LD, InteractionType::AddressRegister_Register, RegisterType::HL, RegisterType::L},
    // 0x76
    Instruction{},
    // 0x77
    Instruction{InstructionType::LD, InteractionType::AddressRegister_Register, RegisterType::HL, RegisterType::A},
    // 0x78
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::A, RegisterType::B},
    // 0x79
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::A, RegisterType::C},
    // 0x7A
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::A, RegisterType::D},
    // 0x7B
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::A, RegisterType::E},
    // 0x7C
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::A, RegisterType::H},
    // 0x7D
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::A, RegisterType::L},
    // 0x7E
    Instruction{InstructionType::LD, InteractionType::Register_AddressRegister, RegisterType::A, RegisterType::HL},
    // 0x7F
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::A, RegisterType::A},
    // 0x80
    Instruction{},
    // 0x81
    Instruction{},
    // 0x82
    Instruction{},
    // 0x83
    Instruction{},
    // 0x84
    Instruction{},
    // 0x85
    Instruction{},
    // 0x86
    Instruction{},
    // 0x87
    Instruction{},
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
    Instruction{},
    // 0x91
    Instruction{},
    // 0x92
    Instruction{},
    // 0x93
    Instruction{},
    // 0x94
    Instruction{},
    // 0x95
    Instruction{},
    // 0x96
    Instruction{},
    // 0x97
    Instruction{},
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
    Instruction{InstructionType::XOR, InteractionType::Register_Register, RegisterType::A, RegisterType::B},
    // 0xA9
    Instruction{InstructionType::XOR, InteractionType::Register_Register, RegisterType::A, RegisterType::C},
    // 0xAA
    Instruction{InstructionType::XOR, InteractionType::Register_Register, RegisterType::A, RegisterType::D},
    // 0xAB
    Instruction{InstructionType::XOR, InteractionType::Register_Register, RegisterType::A, RegisterType::E},
    // 0xAC
    Instruction{InstructionType::XOR, InteractionType::Register_Register, RegisterType::A, RegisterType::H},
    // 0xAD
    Instruction{InstructionType::XOR, InteractionType::Register_Register, RegisterType::A, RegisterType::L},
    // 0xAE
    Instruction{},
    // 0xAF
    Instruction{InstructionType::XOR, InteractionType::Register_Register, RegisterType::A, RegisterType::A},
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
    Instruction{},
    // 0xB9
    Instruction{},
    // 0xBA
    Instruction{},
    // 0xBB
    Instruction{},
    // 0xBC
    Instruction{},
    // 0xBD
    Instruction{},
    // 0xBE
    Instruction{},
    // 0xBF
    Instruction{},
    // 0xC0
    Instruction{},
    // 0xC1
    Instruction{InstructionType::POP, InteractionType::None, RegisterType::BC},
    // 0xC2
    Instruction{},
    // 0xC3
    Instruction{},
    // 0xC4
    Instruction{InstructionType::CALL, InteractionType::ImmediateWord, RegisterType::None, RegisterType::None, ConditionType::NonZero},
    // 0xC5
    Instruction{InstructionType::PUSH, InteractionType::None, RegisterType::BC},
    // 0xC6
    Instruction{},
    // 0xC7
    Instruction{},
    // 0xC8
    Instruction{},
    // 0xC9
    Instruction{},
    // 0xCA
    Instruction{},
    // 0xCB
    Instruction{InstructionType::CB, InteractionType::ImmediateByte},
    // 0xCC
    Instruction{InstructionType::CALL, InteractionType::ImmediateWord, RegisterType::None, RegisterType::None, ConditionType::Zero},
    // 0xCD
    Instruction{InstructionType::CALL, InteractionType::ImmediateWord},
    // 0xCE
    Instruction{},
    // 0xCF
    Instruction{},
    // 0xD0
    Instruction{},
    // 0xD1
    Instruction{InstructionType::POP, InteractionType::None, RegisterType::DE},
    // 0xD2
    Instruction{},
    // 0xD3
    Instruction{},
    // 0xD4
    Instruction{InstructionType::CALL, InteractionType::ImmediateWord, RegisterType::None, RegisterType::None, ConditionType::NonCarry},
    // 0xD5
    Instruction{InstructionType::PUSH, InteractionType::None, RegisterType::DE},
    // 0xD6
    Instruction{},
    // 0xD7
    Instruction{},
    // 0xD8
    Instruction{},
    // 0xD9
    Instruction{},
    // 0xDA
    Instruction{},
    // 0xDB
    Instruction{},
    // 0xDC
    Instruction{InstructionType::CALL, InteractionType::ImmediateWord, RegisterType::None, RegisterType::None, ConditionType::Carry},
    // 0xDD
    Instruction{},
    // 0xDE
    Instruction{},
    // 0xDF
    Instruction{},
    // 0xE0
    Instruction{InstructionType::LDH, InteractionType::ImmediateByte, RegisterType::None, RegisterType::A},
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
    Instruction{InstructionType::LD, InteractionType::AddressWord_Register, RegisterType::None, RegisterType::A},
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
    Instruction{InstructionType::LDHL, InteractionType::ImmediateByte, RegisterType::HL, RegisterType::SP},
    // 0xF9
    Instruction{InstructionType::LD, InteractionType::Register_Register, RegisterType::SP, RegisterType::HL},
    // 0xFA
    Instruction{InstructionType::LD, InteractionType::Register_AddressWord, RegisterType::A},
    // 0xFB
    Instruction{},
    // 0xFC
    Instruction{},
    // 0xFD
    Instruction{},
    // 0xFE
    Instruction{},
    // 0xFF
    Instruction{}
};

inline Instruction get_instruction_by_value(uint8_t value) {
    return instructions[value];
}


struct OpCode {
        uint16_t value;
        std::string_view label;
        int immediate_data;
        bool extended() const {return bitmanip::get_high_byte(value) == 0xCB;}
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
    constexpr OpCode LDH_A_N{0xF0};
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
    // Compare immediate value or register against register
    constexpr OpCode CP_N{0xFE};
    constexpr OpCode CP_A{0xBF};
    constexpr OpCode CP_B{0xB8};
    constexpr OpCode CP_C{0xB9};
    constexpr OpCode CP_D{0xBA};
    constexpr OpCode CP_E{0xBB};
    constexpr OpCode CP_H{0xBC};
    constexpr OpCode CP_L{0xBD};
    constexpr OpCode CP_HL{0xBE};
    constexpr OpCode LD_NN_A{0xEA};
    constexpr OpCode LD_A_NN{0xFA};
    constexpr OpCode SUB_A_B{0x90};
    constexpr OpCode SUB_A_C{0x91};
    constexpr OpCode SUB_A_D{0x92};
    constexpr OpCode SUB_A_E{0x93};
    constexpr OpCode SUB_A_H{0x94};
    constexpr OpCode SUB_A_L{0x95};
    constexpr OpCode SUB_A_HL{0x96};
    constexpr OpCode SUB_A_A{0x97};
    constexpr OpCode ADD_A_B{0x80};
    constexpr OpCode ADD_A_C{0x81};
    constexpr OpCode ADD_A_D{0x82};
    constexpr OpCode ADD_A_E{0x83};
    constexpr OpCode ADD_A_H{0x84};
    constexpr OpCode ADD_A_L{0x85};
    constexpr OpCode ADD_A_HL{0x86};
    constexpr OpCode ADD_A_A{0x87};
    // Unconditional jump
    constexpr OpCode JP_NN{0xC3};
    // Disable interrupts
    constexpr OpCode DI{0xF3};

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
        if (opcode.extended()) {
            return format_to(context.out(), "0xCB {:02X}", opcode.value);
        }
        return format_to(context.out(), "{:02X}", opcode.value);
    }
};


/**
 * Formatted output for Instructions
 */
template <>
struct fmt::formatter<opcodes::Instruction> {

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
