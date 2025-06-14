#include "opcodes.hpp"

namespace opcodes {

constexpr std::array<Instruction, 0x100> instructions{
    // 0x00
    Instruction{.instruction_type = InstructionType::NOP},
    // 0x01 Load 16 bit immediate into BC
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::ImmediateWord,
                .register_type_destination = RegisterType::BC},
    // 0x02 Save A to address pointed by BC
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::AddressRegister_Register,
                .register_type_destination = RegisterType::BC,
                .register_type_source = RegisterType::A},
    // 0x03
    Instruction{.instruction_type = InstructionType::INC,
                .interaction_type = InteractionType::Register,
                .register_type_destination = RegisterType::BC},
    // 0x04
    Instruction{.instruction_type = InstructionType::INC,
                .interaction_type = InteractionType::Register,
                .register_type_destination = RegisterType::B},
    // 0x05
    Instruction{.instruction_type = InstructionType::DEC,
                .interaction_type = InteractionType::Register,
                .register_type_destination = RegisterType::B},
    // 0x06 Load immediate byte into B
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::ImmediateByte,
                .register_type_destination = RegisterType::B},
    // 0x07 Rotate A left with carry
    Instruction{.instruction_type = InstructionType::RLC,
                .interaction_type = InteractionType::Register,
                .register_type_destination = RegisterType::A},
    // 0x08 Save SP to address given by immediate word
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::AddressWord_Register,
                .register_type_destination = RegisterType::None,
                .register_type_source = RegisterType::SP},
    // 0x09 Add BC to HL
    Instruction{.instruction_type = InstructionType::ADD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::HL,
                .register_type_source = RegisterType::BC},
    // 0x0A Load A from address pointed to by BC
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_AddressRegister,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::BC},
    // 0x0B
    Instruction{.instruction_type = InstructionType::DEC,
                .interaction_type = InteractionType::Register,
                .register_type_destination = RegisterType::BC},
    // 0x0C
    Instruction{.instruction_type = InstructionType::INC,
                .interaction_type = InteractionType::Register,
                .register_type_destination = RegisterType::C},
    // 0x0D
    Instruction{.instruction_type = InstructionType::DEC,
                .interaction_type = InteractionType::Register,
                .register_type_destination = RegisterType::C},
    // 0x0E
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::ImmediateByte,
                .register_type_destination = RegisterType::C},
    // 0x0F
    Instruction{.instruction_type = InstructionType::RRC,
                .interaction_type = InteractionType::Register,
                .register_type_destination = RegisterType::A},
    // 0x10 STOP
    Instruction{.instruction_type = InstructionType::STOP},
    // 0x11
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::ImmediateWord,
                .register_type_destination = RegisterType::DE},
    // 0x12
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::AddressRegister_Register,
                .register_type_destination = RegisterType::DE,
                .register_type_source = RegisterType::A},
    // 0x13
    Instruction{.instruction_type = InstructionType::INC,
                .interaction_type = InteractionType::Register,
                .register_type_destination = RegisterType::DE},
    // 0x14
    Instruction{.instruction_type = InstructionType::INC,
                .interaction_type = InteractionType::Register,
                .register_type_destination = RegisterType::D},
    // 0x15
    Instruction{.instruction_type = InstructionType::DEC,
                .interaction_type = InteractionType::Register,
                .register_type_destination = RegisterType::D},
    // 0x16
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::ImmediateByte,
                .register_type_destination = RegisterType::D},
    // 0x17
    Instruction{.instruction_type = InstructionType::RL,
                .interaction_type = InteractionType::Register,
                .register_type_destination = RegisterType::A},
    // 0x18
    Instruction{.instruction_type = InstructionType::JR,
                .interaction_type = InteractionType::ImmediateByte},
    // 0x19
    Instruction{.instruction_type = InstructionType::ADD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::HL,
                .register_type_source = RegisterType::DE},
    // 0x1A
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_AddressRegister,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::DE},
    // 0x1B
    Instruction{.instruction_type = InstructionType::DEC,
                .interaction_type = InteractionType::Register,
                .register_type_destination = RegisterType::DE},
    // 0x1C
    Instruction{.instruction_type = InstructionType::INC,
                .interaction_type = InteractionType::Register,
                .register_type_destination = RegisterType::E},
    // 0x1D
    Instruction{.instruction_type = InstructionType::DEC,
                .interaction_type = InteractionType::Register,
                .register_type_destination = RegisterType::E},
    // 0x1E
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::ImmediateByte,
                .register_type_destination = RegisterType::E},
    // 0x1F
    Instruction{.instruction_type = InstructionType::RR,
                .interaction_type = InteractionType::Register,
                .register_type_destination = RegisterType::A},
    // 0x20 Jump if last result was non zero
    Instruction{.instruction_type = InstructionType::JR,
                .interaction_type = InteractionType::ImmediateByte,
                .register_type_destination = RegisterType::None,
                .register_type_source = RegisterType::None,
                .condition_type = ConditionType::NonZero},
    // 0x21
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::ImmediateWord,
                .register_type_destination = RegisterType::HL},
    // 0x22
    Instruction{.instruction_type = InstructionType::LDI,
                .interaction_type = InteractionType::AddressRegister_Register,
                .register_type_destination = RegisterType::HL,
                .register_type_source = RegisterType::A},
    // 0x23
    Instruction{.instruction_type = InstructionType::INC,
                .interaction_type = InteractionType::Register,
                .register_type_destination = RegisterType::HL},
    // 0x24
    Instruction{.instruction_type = InstructionType::INC,
                .interaction_type = InteractionType::Register,
                .register_type_destination = RegisterType::H},
    // 0x25
    Instruction{.instruction_type = InstructionType::DEC,
                .interaction_type = InteractionType::Register,
                .register_type_destination = RegisterType::H},
    // 0x26
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::ImmediateByte,
                .register_type_destination = RegisterType::H},
    // 0x27 This instruction only exists for register A, so we don't have to specify the register
    // here
    Instruction{.instruction_type = InstructionType::DAA},
    // 0x28 Jump if last result was zero
    Instruction{.instruction_type = InstructionType::JR,
                .interaction_type = InteractionType::ImmediateByte,
                .register_type_destination = RegisterType::None,
                .register_type_source = RegisterType::None,
                .condition_type = ConditionType::Zero},
    // 0x29
    Instruction{.instruction_type = InstructionType::ADD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::HL,
                .register_type_source = RegisterType::HL},
    // 0x2A
    Instruction{.instruction_type = InstructionType::LDI,
                .interaction_type = InteractionType::Register_AddressRegister,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::HL},
    // 0x2B
    Instruction{.instruction_type = InstructionType::DEC,
                .interaction_type = InteractionType::Register,
                .register_type_destination = RegisterType::HL},
    // 0x2C
    Instruction{.instruction_type = InstructionType::INC,
                .interaction_type = InteractionType::Register,
                .register_type_destination = RegisterType::L},
    // 0x2D
    Instruction{.instruction_type = InstructionType::DEC,
                .interaction_type = InteractionType::Register,
                .register_type_destination = RegisterType::L},
    // 0x2E
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::ImmediateByte,
                .register_type_destination = RegisterType::L},
    // 0x2F This instruction only exists for register A, so we don't have to specify the register
    // here
    Instruction{.instruction_type = InstructionType::CPL},
    // 0x30
    Instruction{.instruction_type = InstructionType::JR,
                .interaction_type = InteractionType::ImmediateByte,
                .register_type_destination = RegisterType::None,
                .register_type_source = RegisterType::None,
                .condition_type = ConditionType::NonCarry},
    // 0x31
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::ImmediateWord,
                .register_type_destination = RegisterType::SP},
    // 0x32 Save A to address pointed at by HL and decrement HL
    Instruction{.instruction_type = InstructionType::LDD,
                .interaction_type = InteractionType::AddressRegister_Register,
                .register_type_destination = RegisterType::HL,
                .register_type_source = RegisterType::A},
    // 0x33
    Instruction{.instruction_type = InstructionType::INC,
                .interaction_type = InteractionType::Register,
                .register_type_destination = RegisterType::SP},
    // 0x34
    Instruction{.instruction_type = InstructionType::INC,
                .interaction_type = InteractionType::AddressRegister,
                .register_type_destination = RegisterType::HL},
    // 0x35
    Instruction{.instruction_type = InstructionType::DEC,
                .interaction_type = InteractionType::AddressRegister,
                .register_type_destination = RegisterType::HL},
    // 0x36
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::AddressRegister_ImmediateByte,
                .register_type_destination = RegisterType::HL},
    // 0x37
    Instruction{.instruction_type = InstructionType::SCF},
    // 0x38
    Instruction{.instruction_type = InstructionType::JR,
                .interaction_type = InteractionType::ImmediateByte,
                .register_type_destination = RegisterType::None,
                .register_type_source = RegisterType::None,
                .condition_type = ConditionType::Carry},
    // 0x39
    Instruction{.instruction_type = InstructionType::ADD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::HL,
                .register_type_source = RegisterType::SP},
    // 0x3A
    Instruction{.instruction_type = InstructionType::LDD,
                .interaction_type = InteractionType::Register_AddressRegister,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::HL},
    // 0x3B
    Instruction{.instruction_type = InstructionType::DEC,
                .interaction_type = InteractionType::Register,
                .register_type_destination = RegisterType::SP},
    // 0x3C
    Instruction{.instruction_type = InstructionType::INC,
                .interaction_type = InteractionType::Register,
                .register_type_destination = RegisterType::A},
    // 0x3D
    Instruction{.instruction_type = InstructionType::DEC,
                .interaction_type = InteractionType::Register,
                .register_type_destination = RegisterType::A},
    // 0x3E
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::ImmediateByte,
                .register_type_destination = RegisterType::A},
    // 0x3F
    Instruction{.instruction_type = InstructionType::CCF},
    // 0x40
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::B,
                .register_type_source = RegisterType::B},
    // 0x41
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::B,
                .register_type_source = RegisterType::C},
    // 0x42
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::B,
                .register_type_source = RegisterType::D},
    // 0x43
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::B,
                .register_type_source = RegisterType::E},
    // 0x44
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::B,
                .register_type_source = RegisterType::H},
    // 0x45
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::B,
                .register_type_source = RegisterType::L},
    // 0x46
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_AddressRegister,
                .register_type_destination = RegisterType::B,
                .register_type_source = RegisterType::HL},
    // 0x47
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::B,
                .register_type_source = RegisterType::A},
    // 0x48
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::C,
                .register_type_source = RegisterType::B},
    // 0x49
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::C,
                .register_type_source = RegisterType::C},
    // 0x4A
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::C,
                .register_type_source = RegisterType::D},
    // 0x4B
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::C,
                .register_type_source = RegisterType::E},
    // 0x4C
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::C,
                .register_type_source = RegisterType::H},
    // 0x4D
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::C,
                .register_type_source = RegisterType::L},
    // 0x4E
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_AddressRegister,
                .register_type_destination = RegisterType::C,
                .register_type_source = RegisterType::HL},
    // 0x4F
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::C,
                .register_type_source = RegisterType::A},
    // 0x50
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::D,
                .register_type_source = RegisterType::B},
    // 0x51
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::D,
                .register_type_source = RegisterType::C},
    // 0x52
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::D,
                .register_type_source = RegisterType::D},
    // 0x53
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::D,
                .register_type_source = RegisterType::E},
    // 0x54
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::D,
                .register_type_source = RegisterType::H},
    // 0x55
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::D,
                .register_type_source = RegisterType::L},
    // 0x56
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_AddressRegister,
                .register_type_destination = RegisterType::D,
                .register_type_source = RegisterType::HL},
    // 0x57
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::D,
                .register_type_source = RegisterType::A},
    // 0x58
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::E,
                .register_type_source = RegisterType::B},
    // 0x59
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::E,
                .register_type_source = RegisterType::C},
    // 0x5A
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::E,
                .register_type_source = RegisterType::D},
    // 0x5B
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::E,
                .register_type_source = RegisterType::E},
    // 0x5C
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::E,
                .register_type_source = RegisterType::H},
    // 0x5D
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::E,
                .register_type_source = RegisterType::L},
    // 0x5E
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_AddressRegister,
                .register_type_destination = RegisterType::E,
                .register_type_source = RegisterType::HL},
    // 0x5F
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::E,
                .register_type_source = RegisterType::A},
    // 0x60
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::H,
                .register_type_source = RegisterType::B},
    // 0x61
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::H,
                .register_type_source = RegisterType::C},
    // 0x62
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::H,
                .register_type_source = RegisterType::D},
    // 0x63
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::H,
                .register_type_source = RegisterType::E},
    // 0x64
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::H,
                .register_type_source = RegisterType::H},
    // 0x65
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::H,
                .register_type_source = RegisterType::L},
    // 0x66
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_AddressRegister,
                .register_type_destination = RegisterType::H,
                .register_type_source = RegisterType::HL},
    // 0x67
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::H,
                .register_type_source = RegisterType::A},
    // 0x68
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::L,
                .register_type_source = RegisterType::B},
    // 0x69
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::L,
                .register_type_source = RegisterType::C},
    // 0x6A
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::L,
                .register_type_source = RegisterType::D},
    // 0x6B
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::L,
                .register_type_source = RegisterType::E},
    // 0x6C
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::L,
                .register_type_source = RegisterType::H},
    // 0x6D
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::L,
                .register_type_source = RegisterType::L},
    // 0x6E
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_AddressRegister,
                .register_type_destination = RegisterType::L,
                .register_type_source = RegisterType::HL},
    // 0x6F
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::L,
                .register_type_source = RegisterType::A},
    // 0x70
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::AddressRegister_Register,
                .register_type_destination = RegisterType::HL,
                .register_type_source = RegisterType::B},
    // 0x71
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::AddressRegister_Register,
                .register_type_destination = RegisterType::HL,
                .register_type_source = RegisterType::C},
    // 0x72
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::AddressRegister_Register,
                .register_type_destination = RegisterType::HL,
                .register_type_source = RegisterType::D},
    // 0x73
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::AddressRegister_Register,
                .register_type_destination = RegisterType::HL,
                .register_type_source = RegisterType::E},
    // 0x74
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::AddressRegister_Register,
                .register_type_destination = RegisterType::HL,
                .register_type_source = RegisterType::H},
    // 0x75
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::AddressRegister_Register,
                .register_type_destination = RegisterType::HL,
                .register_type_source = RegisterType::L},
    // 0x76
    Instruction{.instruction_type = InstructionType::HALT},
    // 0x77
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::AddressRegister_Register,
                .register_type_destination = RegisterType::HL,
                .register_type_source = RegisterType::A},
    // 0x78
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::B},
    // 0x79
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::C},
    // 0x7A
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::D},
    // 0x7B
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::E},
    // 0x7C
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::H},
    // 0x7D
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::L},
    // 0x7E
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_AddressRegister,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::HL},
    // 0x7F
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::A},
    // 0x80
    Instruction{.instruction_type = InstructionType::ADD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::B},
    // 0x81
    Instruction{.instruction_type = InstructionType::ADD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::C},
    // 0x82
    Instruction{.instruction_type = InstructionType::ADD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::D},
    // 0x83
    Instruction{.instruction_type = InstructionType::ADD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::E},
    // 0x84
    Instruction{.instruction_type = InstructionType::ADD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::H},
    // 0x85
    Instruction{.instruction_type = InstructionType::ADD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::L},
    // 0x86
    Instruction{.instruction_type = InstructionType::ADD,
                .interaction_type = InteractionType::Register_AddressRegister,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::HL},
    // 0x87
    Instruction{.instruction_type = InstructionType::ADD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::A},
    // 0x88
    Instruction{.instruction_type = InstructionType::ADC,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::B},
    // 0x89
    Instruction{.instruction_type = InstructionType::ADC,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::C},
    // 0x8A
    Instruction{.instruction_type = InstructionType::ADC,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::D},
    // 0x8B
    Instruction{.instruction_type = InstructionType::ADC,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::E},
    // 0x8C
    Instruction{.instruction_type = InstructionType::ADC,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::H},
    // 0x8D
    Instruction{.instruction_type = InstructionType::ADC,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::L},
    // 0x8E
    Instruction{.instruction_type = InstructionType::ADC,
                .interaction_type = InteractionType::Register_AddressRegister,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::HL},
    // 0x8F
    Instruction{.instruction_type = InstructionType::ADC,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::A},
    // 0x90
    Instruction{.instruction_type = InstructionType::SUB,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::B},
    // 0x91
    Instruction{.instruction_type = InstructionType::SUB,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::C},
    // 0x92
    Instruction{.instruction_type = InstructionType::SUB,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::D},
    // 0x93
    Instruction{.instruction_type = InstructionType::SUB,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::E},
    // 0x94
    Instruction{.instruction_type = InstructionType::SUB,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::H},
    // 0x95
    Instruction{.instruction_type = InstructionType::SUB,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::L},
    // 0x96
    Instruction{.instruction_type = InstructionType::SUB,
                .interaction_type = InteractionType::Register_AddressRegister,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::HL},
    // 0x97
    Instruction{.instruction_type = InstructionType::SUB,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::A},
    // 0x98
    Instruction{.instruction_type = InstructionType::SBC,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::B},
    // 0x99
    Instruction{.instruction_type = InstructionType::SBC,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::C},
    // 0x9A
    Instruction{.instruction_type = InstructionType::SBC,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::D},
    // 0x9B
    Instruction{.instruction_type = InstructionType::SBC,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::E},
    // 0x9C
    Instruction{.instruction_type = InstructionType::SBC,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::H},
    // 0x9D
    Instruction{.instruction_type = InstructionType::SBC,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::L},
    // 0x9E
    Instruction{.instruction_type = InstructionType::SBC,
                .interaction_type = InteractionType::Register_AddressRegister,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::HL},
    // 0x9F
    Instruction{.instruction_type = InstructionType::SBC,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::A},
    // 0xA0
    Instruction{.instruction_type = InstructionType::AND,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::B},
    // 0xA1
    Instruction{.instruction_type = InstructionType::AND,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::C},
    // 0xA2
    Instruction{.instruction_type = InstructionType::AND,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::D},
    // 0xA3
    Instruction{.instruction_type = InstructionType::AND,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::E},
    // 0xA4
    Instruction{.instruction_type = InstructionType::AND,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::H},
    // 0xA5
    Instruction{.instruction_type = InstructionType::AND,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::L},
    // 0xA6
    Instruction{.instruction_type = InstructionType::AND,
                .interaction_type = InteractionType::Register_AddressRegister,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::HL},
    // 0xA7
    Instruction{.instruction_type = InstructionType::AND,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::A},
    // 0xA8 A = A XOR B
    Instruction{.instruction_type = InstructionType::XOR,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::B},
    // 0xA9
    Instruction{.instruction_type = InstructionType::XOR,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::C},
    // 0xAA
    Instruction{.instruction_type = InstructionType::XOR,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::D},
    // 0xAB
    Instruction{.instruction_type = InstructionType::XOR,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::E},
    // 0xAC
    Instruction{.instruction_type = InstructionType::XOR,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::H},
    // 0xAD
    Instruction{.instruction_type = InstructionType::XOR,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::L},
    // 0xAE
    Instruction{.instruction_type = InstructionType::XOR,
                .interaction_type = InteractionType::Register_AddressRegister,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::HL},
    // 0xAF
    Instruction{.instruction_type = InstructionType::XOR,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::A},
    // 0xB0
    Instruction{.instruction_type = InstructionType::OR,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::B},
    // 0xB1
    Instruction{.instruction_type = InstructionType::OR,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::C},
    // 0xB2
    Instruction{.instruction_type = InstructionType::OR,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::D},
    // 0xB3
    Instruction{.instruction_type = InstructionType::OR,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::E},
    // 0xB4
    Instruction{.instruction_type = InstructionType::OR,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::H},
    // 0xB5
    Instruction{.instruction_type = InstructionType::OR,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::L},
    // 0xB6
    Instruction{.instruction_type = InstructionType::OR,
                .interaction_type = InteractionType::Register_AddressRegister,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::HL},
    // 0xB7
    Instruction{.instruction_type = InstructionType::OR,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::A},
    // 0xB8
    Instruction{.instruction_type = InstructionType::CP,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::B},
    // 0xB9
    Instruction{.instruction_type = InstructionType::CP,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::C},
    // 0xBA
    Instruction{.instruction_type = InstructionType::CP,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::D},
    // 0xBB
    Instruction{.instruction_type = InstructionType::CP,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::E},
    // 0xBC
    Instruction{.instruction_type = InstructionType::CP,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::H},
    // 0xBD
    Instruction{.instruction_type = InstructionType::CP,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::L},
    // 0xBE
    Instruction{.instruction_type = InstructionType::CP,
                .interaction_type = InteractionType::Register_AddressRegister,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::HL},
    // 0xBF
    Instruction{.instruction_type = InstructionType::CP,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::A},
    // 0xC0
    Instruction{.instruction_type = InstructionType::RET,
                .interaction_type = InteractionType::None,
                .register_type_destination = RegisterType::None,
                .register_type_source = RegisterType::None,
                .condition_type = ConditionType::NonZero},
    // 0xC1
    Instruction{.instruction_type = InstructionType::POP,
                .interaction_type = InteractionType::None,
                .register_type_destination = RegisterType::BC},
    // 0xC2
    Instruction{.instruction_type = InstructionType::JP,
                .interaction_type = InteractionType::ImmediateWord,
                .register_type_destination = RegisterType::None,
                .register_type_source = RegisterType::None,
                .condition_type = ConditionType::NonZero},
    // 0xC3
    Instruction{.instruction_type = InstructionType::JP,
                .interaction_type = InteractionType::ImmediateWord},
    // 0xC4
    Instruction{.instruction_type = InstructionType::CALL,
                .interaction_type = InteractionType::ImmediateWord,
                .register_type_destination = RegisterType::None,
                .register_type_source = RegisterType::None,
                .condition_type = ConditionType::NonZero},
    // 0xC5
    Instruction{.instruction_type = InstructionType::PUSH,
                .interaction_type = InteractionType::None,
                .register_type_destination = RegisterType::BC},
    // 0xC6
    Instruction{.instruction_type = InstructionType::ADD,
                .interaction_type = InteractionType::ImmediateByte,
                .register_type_destination = RegisterType::A},
    // 0xC7
    Instruction{.instruction_type = InstructionType::RST},
    // 0xC8
    Instruction{.instruction_type = InstructionType::RET,
                .interaction_type = InteractionType::None,
                .register_type_destination = RegisterType::None,
                .register_type_source = RegisterType::None,
                .condition_type = ConditionType::Zero},
    // 0xC9
    Instruction{.instruction_type = InstructionType::RET},
    // 0xCA
    Instruction{.instruction_type = InstructionType::JP,
                .interaction_type = InteractionType::ImmediateWord,
                .register_type_destination = RegisterType::None,
                .register_type_source = RegisterType::None,
                .condition_type = ConditionType::Zero},
    // 0xCB
    Instruction{.instruction_type = InstructionType::CB,
                .interaction_type = InteractionType::ImmediateByte},
    // 0xCC
    Instruction{.instruction_type = InstructionType::CALL,
                .interaction_type = InteractionType::ImmediateWord,
                .register_type_destination = RegisterType::None,
                .register_type_source = RegisterType::None,
                .condition_type = ConditionType::Zero},
    // 0xCD
    Instruction{.instruction_type = InstructionType::CALL,
                .interaction_type = InteractionType::ImmediateWord},
    // 0xCE
    Instruction{.instruction_type = InstructionType::ADC,
                .interaction_type = InteractionType::ImmediateByte,
                .register_type_destination = RegisterType::A},
    // 0xCF
    Instruction{.instruction_type = InstructionType::RST},
    // 0xD0
    Instruction{.instruction_type = InstructionType::RET,
                .interaction_type = InteractionType::None,
                .register_type_destination = RegisterType::None,
                .register_type_source = RegisterType::None,
                .condition_type = ConditionType::NonCarry},
    // 0xD1
    Instruction{.instruction_type = InstructionType::POP,
                .interaction_type = InteractionType::None,
                .register_type_destination = RegisterType::DE},
    // 0xD2
    Instruction{.instruction_type = InstructionType::JP,
                .interaction_type = InteractionType::ImmediateWord,
                .register_type_destination = RegisterType::None,
                .register_type_source = RegisterType::None,
                .condition_type = ConditionType::NonCarry},
    // 0xD3
    Instruction{},
    // 0xD4
    Instruction{.instruction_type = InstructionType::CALL,
                .interaction_type = InteractionType::ImmediateWord,
                .register_type_destination = RegisterType::None,
                .register_type_source = RegisterType::None,
                .condition_type = ConditionType::NonCarry},
    // 0xD5
    Instruction{.instruction_type = InstructionType::PUSH,
                .interaction_type = InteractionType::None,
                .register_type_destination = RegisterType::DE},
    // 0xD6
    Instruction{.instruction_type = InstructionType::SUB,
                .interaction_type = InteractionType::ImmediateByte,
                .register_type_destination = RegisterType::A},
    // 0xD7
    Instruction{.instruction_type = InstructionType::RST},
    // 0xD8
    Instruction{.instruction_type = InstructionType::RET,
                .interaction_type = InteractionType::None,
                .register_type_destination = RegisterType::None,
                .register_type_source = RegisterType::None,
                .condition_type = ConditionType::Carry},
    // 0xD9
    Instruction{.instruction_type = InstructionType::RETI},
    // 0xDA
    Instruction{.instruction_type = InstructionType::JP,
                .interaction_type = InteractionType::ImmediateWord,
                .register_type_destination = RegisterType::None,
                .register_type_source = RegisterType::None,
                .condition_type = ConditionType::Carry},
    // 0xDB
    Instruction{},
    // 0xDC
    Instruction{.instruction_type = InstructionType::CALL,
                .interaction_type = InteractionType::ImmediateWord,
                .register_type_destination = RegisterType::None,
                .register_type_source = RegisterType::None,
                .condition_type = ConditionType::Carry},
    // 0xDD
    Instruction{},
    // 0xDE
    Instruction{.instruction_type = InstructionType::SBC,
                .interaction_type = InteractionType::ImmediateByte,
                .register_type_destination = RegisterType::A},
    // 0xDF
    Instruction{.instruction_type = InstructionType::RST},
    // 0xE0
    Instruction{.instruction_type = InstructionType::LDH,
                .interaction_type = InteractionType::ImmediateByte,
                .register_type_destination = RegisterType::None,
                .register_type_source = RegisterType::A},
    // 0xE1
    Instruction{.instruction_type = InstructionType::POP,
                .interaction_type = InteractionType::None,
                .register_type_destination = RegisterType::HL},
    // 0xE2
    Instruction{.instruction_type = InstructionType::LDH,
                .interaction_type = InteractionType::None,
                .register_type_destination = RegisterType::C,
                .register_type_source = RegisterType::A},
    // 0xE3
    Instruction{},
    // 0xE4
    Instruction{},
    // 0xE5
    Instruction{.instruction_type = InstructionType::PUSH,
                .interaction_type = InteractionType::None,
                .register_type_destination = RegisterType::HL},
    // 0xE6
    Instruction{.instruction_type = InstructionType::AND,
                .interaction_type = InteractionType::ImmediateByte,
                .register_type_destination = RegisterType::A},
    // 0xE7
    Instruction{.instruction_type = InstructionType::RST},
    // 0xE8
    Instruction{.instruction_type = InstructionType::ADD_Signed,
                .interaction_type = InteractionType::ImmediateByte,
                .register_type_destination = RegisterType::SP},
    // 0xE9
    Instruction{.instruction_type = InstructionType::JP,
                .interaction_type = InteractionType::AddressRegister,
                .register_type_destination = RegisterType::HL},
    // 0xEA
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::AddressWord_Register,
                .register_type_destination = RegisterType::None,
                .register_type_source = RegisterType::A},
    // 0xEB
    Instruction{},
    // 0xEC
    Instruction{},
    // 0xED
    Instruction{},
    // 0xEE
    Instruction{.instruction_type = InstructionType::XOR,
                .interaction_type = InteractionType::ImmediateByte,
                .register_type_destination = RegisterType::A},
    // 0xEF
    Instruction{.instruction_type = InstructionType::RST},
    // 0xF0
    Instruction{.instruction_type = InstructionType::LDH,
                .interaction_type = InteractionType::ImmediateByte,
                .register_type_destination = RegisterType::A},
    // 0xF1
    Instruction{.instruction_type = InstructionType::POP,
                .interaction_type = InteractionType::None,
                .register_type_destination = RegisterType::AF},
    // 0xF2
    Instruction{.instruction_type = InstructionType::LDH,
                .interaction_type = InteractionType::None,
                .register_type_destination = RegisterType::A,
                .register_type_source = RegisterType::C},
    // 0xF3
    Instruction{.instruction_type = InstructionType::DI},
    // 0xF4
    Instruction{},
    // 0xF5
    Instruction{.instruction_type = InstructionType::PUSH,
                .interaction_type = InteractionType::None,
                .register_type_destination = RegisterType::AF},
    // 0xF6
    Instruction{.instruction_type = InstructionType::OR,
                .interaction_type = InteractionType::ImmediateByte,
                .register_type_destination = RegisterType::A},
    // 0xF7
    Instruction{.instruction_type = InstructionType::RST},
    // 0xF8
    Instruction{.instruction_type = InstructionType::LDHL,
                .interaction_type = InteractionType::ImmediateByte,
                .register_type_destination = RegisterType::HL,
                .register_type_source = RegisterType::SP},
    // 0xF9
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_Register,
                .register_type_destination = RegisterType::SP,
                .register_type_source = RegisterType::HL},
    // 0xFA
    Instruction{.instruction_type = InstructionType::LD,
                .interaction_type = InteractionType::Register_AddressWord,
                .register_type_destination = RegisterType::A},
    // 0xFB
    Instruction{.instruction_type = InstructionType::EI},
    // 0xFC
    Instruction{},
    // 0xFD
    Instruction{},
    // 0xFE
    Instruction{.instruction_type = InstructionType::CP,
                .interaction_type = InteractionType::ImmediateByte,
                .register_type_destination = RegisterType::A},
    // 0xFF
    Instruction{.instruction_type = InstructionType::RST}};

} // namespace opcodes

opcodes::Instruction opcodes::get_instruction_by_value(uint8_t value) {
    return opcodes::instructions[value];
}
