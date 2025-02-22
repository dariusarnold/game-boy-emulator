#include "cpu.hpp"
#include "addressbus.hpp"
#include "bitmanipulation.hpp"
#include "emulator.hpp"
#include "exceptions.hpp"

#include "spdlog/spdlog.h"


void Cpu::step() {
    auto start_cycles = m_emulator->get_state().cycles_m;
    previous_instruction = current_instruction;
    current_instruction = fetch_instruction();
    auto fetched = fetch_data(current_instruction);
    if (fetched.has_value()) {
        m_logger->debug("Opcode: {:02X} Data {:04X}", current_instruction.opcode, fetched.value());
    } else {
        m_logger->debug("Opcode: {:02X}", current_instruction.opcode);
    }
    auto data = fetched.value_or(0);
    m_logger->debug("Executing {}", current_instruction);
    switch (current_instruction.instruction_type) {
    case opcodes::InstructionType::LD:
    case opcodes::InstructionType::LDD:
    case opcodes::InstructionType::LDI:
        instructionLD(current_instruction, data);
        break;
    case opcodes::InstructionType::LDH:
        instructionLDH(current_instruction, data);
        break;
    case opcodes::InstructionType::LDHL:
        instructionLDHL(data);
        break;
    case opcodes::InstructionType::CB:
        // Here the data is the second byte of the CB instruction.
        instructionCB(data);
        break;
    case opcodes::InstructionType::JR:
        instructionJR(current_instruction, data);
        break;
    case opcodes::InstructionType::JP:
        instructionJP(current_instruction, data);
        break;
    case opcodes::InstructionType::INC:
    case opcodes::InstructionType::DEC:
        instructionINCDEC(current_instruction);
        break;
    case opcodes::InstructionType::CALL:
        instructionCALL(current_instruction, data);
        break;
    case opcodes::InstructionType::PUSH:
        instructionPUSH(current_instruction);
        break;
    case opcodes::InstructionType::RL:
        instructionRL(current_instruction);
        break;
    case opcodes::InstructionType::POP:
        instructionPOP(current_instruction);
        break;
    case opcodes::InstructionType::RET:
        instructionRET(current_instruction);
        break;
    case opcodes::InstructionType::CP:
        instructionCP(current_instruction, data);
        break;
    case opcodes::InstructionType::OR:
    case opcodes::InstructionType::XOR:
    case opcodes::InstructionType::AND:
        instructionANDORXOR(current_instruction, data);
        break;
    case opcodes::InstructionType::SUB:
        instructionSUB(current_instruction, data);
        break;
    case opcodes::InstructionType::ADD:
        instructionADD(current_instruction, data);
        break;
    case opcodes::InstructionType::ADD_Signed:
        instructionADD_Signed(static_cast<int8_t>(data));
        break;
    case opcodes::InstructionType::NOP:
        break;
    case opcodes::InstructionType::DI:
        m_emulator->set_interrupts_enabled(false);
        break;
    case opcodes::InstructionType::EI:
        m_emulator->set_interrupts_enabled(true);
        break;
    case opcodes::InstructionType::RETI:
        instructionRETI();
        break;
    case opcodes::InstructionType::RR:
        instruction_cb_rr(opcodes::RegisterType::A);
        set_zero_flag(BitValues::Inactive);
        break;
    case opcodes::InstructionType::ADC:
        instructionADC(current_instruction, data);
        break;
    case opcodes::InstructionType::SBC:
        instructionSBC(current_instruction, data);
        break;
    case opcodes::InstructionType::DAA:
        instructionDAA();
        break;
    case opcodes::InstructionType::HALT:
        m_emulator->halt();
        break;
    case opcodes::InstructionType::RST:
        instructionRST(current_instruction.opcode);
        break;
    case opcodes::InstructionType::CPL:
        instructionCPL();
        break;
    case opcodes::InstructionType::SCF:
        instructionSCF();
        break;
    case opcodes::InstructionType::CCF:
        instructionCCF();
        break;
    case opcodes::InstructionType::RLC:
        registers.a = instructionRLC(registers.a);
        break;
    case opcodes::InstructionType::RRC:
        registers.a = instructionRRC(registers.a);
        break;
    default:
        abort_execution<NotImplementedError>(
            fmt::format("Instruction type {} not implemented",
                        magic_enum::enum_name(current_instruction.instruction_type)));
    }
    auto end_cycles = m_emulator->get_state().cycles_m;
    m_cycles_previous_instruction = end_cycles - start_cycles;
    m_logger->debug("Took {} M cycles", end_cycles - start_cycles);
    m_emulator->elapse_instruction();
}

void Cpu::run() {
    while (true) {
        step();
    }
}

Cpu::Cpu(Emulator* emulator) : m_emulator(emulator), m_logger(spdlog::get("")) {}

void Cpu::set_subtract_flag(BitValues value) {
    bitmanip::set_bit(registers.f, as_integral(flags::subtract), as_integral(value));
}

void Cpu::set_half_carry_flag(BitValues value) {
    bitmanip::set_bit(registers.f, as_integral(flags::half_carry), as_integral(value));
}

void Cpu::set_zero_flag(BitValues value) {
    bitmanip::set_bit(registers.f, as_integral(flags::zero), as_integral(value));
}

void Cpu::set_carry_flag(BitValues value) {
    bitmanip::set_bit(registers.f, as_integral(flags::carry), as_integral(value));
}

void Cpu::set_zero_flag(bool flag_set) {
    set_zero_flag(flag_set ? BitValues::Active : BitValues::Inactive);
}

void Cpu::set_subtract_flag(bool flag_set) {
    set_subtract_flag(flag_set ? BitValues::Active : BitValues::Inactive);
}

void Cpu::set_half_carry_flag(bool flag_set) {
    set_half_carry_flag(flag_set ? BitValues::Active : BitValues::Inactive);
}

void Cpu::set_carry_flag(bool flag_set) {
    set_carry_flag(flag_set ? BitValues::Active : BitValues::Inactive);
}

bool Cpu::is_flag_set(flags flag) const {
    return bitmanip::is_bit_set(registers.f, as_integral(flag));
}

void Cpu::instruction_cb_test_bit(opcodes::RegisterType register_type, uint8_t position) {
    auto value = cb_fetch_data(register_type);
    if (bitmanip::is_bit_set(value, position)) {
        set_zero_flag(BitValues::Inactive);
    } else {
        set_zero_flag(BitValues::Active);
    }
    set_subtract_flag(BitValues::Inactive);
    set_half_carry_flag(BitValues::Active);
}

opcodes::Instruction Cpu::fetch_instruction() {
    auto byte = m_emulator->get_bus()->read_byte(registers.pc);
    m_emulator->elapse_cycle();
    auto instruction = opcodes::get_instruction_by_value(byte);
    if (instruction.instruction_type == opcodes::InstructionType::NONE) {
        abort_execution<NotImplementedError>(
            fmt::format("Encountered unsupported opcode {:02X} at pc {:04X}", byte, registers.pc));
    }
    registers.pc++;
    instruction.opcode = byte;
    return instruction;
}

std::optional<uint16_t> Cpu::fetch_data(opcodes::Instruction instruction) {
    uint8_t high_byte = 0;
    uint8_t low_byte = 0;
    switch (instruction.interaction_type) {
    case opcodes::InteractionType::None:
    case opcodes::InteractionType::Register:
    case opcodes::InteractionType::AddressRegister:
    case opcodes::InteractionType::Register_Register:
    case opcodes::InteractionType::AddressRegister_Register:
    case opcodes::InteractionType::Register_AddressRegister:
        return std::nullopt;
    case opcodes::InteractionType::ImmediateByte:
    case opcodes::InteractionType::AddressRegister_ImmediateByte:
        low_byte = m_emulator->get_bus()->read_byte(registers.pc);
        registers.pc++;
        m_emulator->elapse_cycle();
        return low_byte;
    case opcodes::InteractionType::ImmediateWord:
    case opcodes::InteractionType::Register_AddressWord:
    case opcodes::InteractionType::AddressWord_Register:
        low_byte = m_emulator->get_bus()->read_byte(registers.pc);
        registers.pc++;
        m_emulator->elapse_cycle();
        high_byte = m_emulator->get_bus()->read_byte(registers.pc);
        registers.pc++;
        m_emulator->elapse_cycle();
        return bitmanip::word_from_bytes(high_byte, low_byte);
    }
    // See comment in instructionJR.
    __builtin_unreachable();
}


std::string Cpu::get_minimal_debug_state() {
    // Format: [registers] (mem[pc] mem[pc+1] mem[pc+2] mem[pc+3])
    // Thanks to https://github.com/wheremyfoodat/Gameboy-logs
    return fmt::format("A: {:02X} F: {:02X} B: {:02X} C: {:02X} D: {:02X} E: {:02X} H: {:02X} L: "
                       "{:02X} SP: {:04X} "
                       "PC: {:04X} ({:02X} {:02X} {:02X} {:02X})",
                       registers.a, registers.f, registers.b, registers.c, registers.d, registers.e,
                       registers.h, registers.l, registers.sp, registers.pc,
                       m_emulator->get_bus()->read_byte(registers.pc),
                       m_emulator->get_bus()->read_byte(registers.pc + 1),
                       m_emulator->get_bus()->read_byte(registers.pc + 2),
                       m_emulator->get_bus()->read_byte(registers.pc + 3));
}

CpuDebugState Cpu::get_debug_state() {
    return {registers.a,
            registers.f,
            registers.b,
            registers.c,
            registers.d,
            registers.e,
            registers.h,
            registers.l,
            registers.sp,
            registers.pc,
            {m_emulator->get_bus()->read_byte(registers.pc),
             m_emulator->get_bus()->read_byte(registers.pc + 1),
             m_emulator->get_bus()->read_byte(registers.pc + 2),
             m_emulator->get_bus()->read_byte(registers.pc + 3)}};
}

opcodes::Instruction Cpu::get_current_instruction() const {
    return current_instruction;
}

opcodes::Instruction Cpu::get_previous_instruction() const {
    return previous_instruction;
}

size_t Cpu::cycle_duration_previous_instruction() const {
    return m_cycles_previous_instruction;
}

void Cpu::set_register_value(opcodes::RegisterType register_type, uint16_t value) {
    switch (register_type) {
    case opcodes::RegisterType::A:
        registers.a = bitmanip::get_low_byte(value);
        return;
    case opcodes::RegisterType::F:
        registers.f = bitmanip::get_low_byte(value);
        return;
    case opcodes::RegisterType::B:
        registers.b = bitmanip::get_low_byte(value);
        return;
    case opcodes::RegisterType::C:
        registers.c = bitmanip::get_low_byte(value);
        return;
    case opcodes::RegisterType::D:
        registers.d = bitmanip::get_low_byte(value);
        return;
    case opcodes::RegisterType::E:
        registers.e = bitmanip::get_low_byte(value);
        return;
    case opcodes::RegisterType::H:
        registers.h = bitmanip::get_low_byte(value);
        return;
    case opcodes::RegisterType::L:
        registers.l = bitmanip::get_low_byte(value);
        return;
    case opcodes::RegisterType::SP:
        registers.sp = value;
        return;
    case opcodes::RegisterType::PC:
        registers.pc = value;
        return;
    case opcodes::RegisterType::AF:
        registers.af = value;
        return;
    case opcodes::RegisterType::BC:
        registers.bc = value;
        return;
    case opcodes::RegisterType::DE:
        registers.de = value;
        return;
    case opcodes::RegisterType::HL:
        registers.hl = value;
        return;
    default:
        abort_execution<LogicError>(
            fmt::format("Unhandled register type {}", magic_enum::enum_name(register_type)));
    }
}

uint16_t Cpu::get_register_value(opcodes::RegisterType register_type) {
    switch (register_type) {
    case opcodes::RegisterType::A:
        return registers.a;
    case opcodes::RegisterType::F:
        return registers.f;
    case opcodes::RegisterType::B:
        return registers.b;
    case opcodes::RegisterType::C:
        return registers.c;
    case opcodes::RegisterType::D:
        return registers.d;
    case opcodes::RegisterType::E:
        return registers.e;
    case opcodes::RegisterType::H:
        return registers.h;
    case opcodes::RegisterType::L:
        return registers.l;
    case opcodes::RegisterType::SP:
        return registers.sp;
    case opcodes::RegisterType::PC:
        return registers.pc;
    case opcodes::RegisterType::AF:
        return registers.af;
    case opcodes::RegisterType::BC:
        return registers.bc;
    case opcodes::RegisterType::DE:
        return registers.de;
    case opcodes::RegisterType::HL:
        return registers.hl;
    case opcodes::RegisterType::None:
        abort_execution<LogicError>(
            fmt::format("Unknown register value {}", magic_enum::enum_name(register_type)));
    }
    // See comment in instructionJR.
    __builtin_unreachable();
}

void Cpu::instructionLD(opcodes::Instruction instruction, uint16_t data) {
    uint16_t value = 0;
    switch (instruction.interaction_type) {
    case opcodes::InteractionType::ImmediateByte:
    case opcodes::InteractionType::ImmediateWord:
        set_register_value(instruction.register_type_destination, data);
        return;
    case opcodes::InteractionType::Register_Register:
        if (instruction.register_type_source == opcodes::RegisterType::B
            && instruction.register_type_destination == opcodes::RegisterType::B) {
            m_emulator->debug();
        }
        value = get_register_value(instruction.register_type_source);
        set_register_value(instruction.register_type_destination, value);
        if (instruction.opcode == 0xF9) {
            // The only word/word LD instruction, it takes one more cycle
            m_emulator->elapse_cycle();
        }
        return;
    case opcodes::InteractionType::AddressRegister_Register:
        // This is actually the address of the destination.
        value = get_register_value(instruction.register_type_destination);
        m_emulator->get_bus()->write_byte(value,
                                          get_register_value(instruction.register_type_source));
        m_emulator->elapse_cycle();
        if (instruction.instruction_type == opcodes::InstructionType::LDD) {
            set_register_value(instruction.register_type_destination, value - 1);
        } else if (instruction.instruction_type == opcodes::InstructionType::LDI) {
            set_register_value(instruction.register_type_destination, value + 1);
        }
        return;
    case opcodes::InteractionType::Register_AddressRegister:
        value = m_emulator->get_bus()->read_byte(
            get_register_value(instruction.register_type_source));
        m_emulator->elapse_cycle();
        set_register_value(instruction.register_type_destination, value);
        if (instruction.instruction_type == opcodes::InstructionType::LDD) {
            set_register_value(instruction.register_type_source,
                               get_register_value(instruction.register_type_source) - 1);
        } else if (instruction.instruction_type == opcodes::InstructionType::LDI) {
            set_register_value(instruction.register_type_source,
                               get_register_value(instruction.register_type_source) + 1);
        }
        return;
    case opcodes::InteractionType::AddressRegister_ImmediateByte:
        // read address register
        value = get_register_value(instruction.register_type_destination);
        m_emulator->get_bus()->write_byte(value, data);
        m_emulator->elapse_cycle();
        return;
    case opcodes::InteractionType::AddressWord_Register:
        // Save register value to address given by immediate word
        switch (instruction.register_type_source) {
        case opcodes::RegisterType::BC:
        case opcodes::RegisterType::DE:
        case opcodes::RegisterType::HL:
        case opcodes::RegisterType::SP:
            value = get_register_value(instruction.register_type_source);
            m_emulator->get_bus()->write_byte(data, bitmanip::get_low_byte(value));
            m_emulator->elapse_cycle();
            m_emulator->get_bus()->write_byte(data + 1, bitmanip::get_high_byte(value));
            m_emulator->elapse_cycle();
            break;
        default:
            m_emulator->get_bus()->write_byte(data,
                                              get_register_value(instruction.register_type_source));
            m_emulator->elapse_cycle();
            break;
        }
        return;
    case opcodes::InteractionType::Register_AddressWord:
        value = m_emulator->get_bus()->read_byte(data);
        m_emulator->elapse_cycle();
        set_register_value(instruction.register_type_destination, value);
        return;
    default:
        abort_execution<NotImplementedError>(
            fmt::format("LD with InteractionType {} not implemented",
                        magic_enum::enum_name(instruction.interaction_type)));
    }
}

void Cpu::instructionLDH(opcodes::Instruction instruction, uint16_t data) {
    const uint16_t offset = 0xFF00;
    if (instruction.interaction_type == opcodes::InteractionType::None) {
        // For other instruction type (ImmediateByte), data already contains the address offset
        data = registers.c;
    }
    if (instruction.register_type_source == opcodes::RegisterType::A) {
        // 0xE0/0xE2 Store A at (FF00+u8/C)
        m_emulator->get_bus()->write_byte(data + offset, registers.a);
    } else {
        // 0xF0/0xF2 Read A from (FF00+u8/C)
        registers.a = m_emulator->get_bus()->read_byte(data + offset);
    }
    m_emulator->elapse_cycle();
}

void Cpu::instructionLDHL(uint8_t data) {
    registers.hl = registers.sp + static_cast<int8_t>(data);
    set_zero_flag(BitValues::Inactive);
    set_subtract_flag(BitValues::Inactive);
    auto hc = (registers.sp ^ static_cast<int8_t>(data) ^ registers.hl) & 0x10;
    set_half_carry_flag(hc > 0);
    auto cf = (registers.sp ^ static_cast<int8_t>(data) ^ registers.hl) & 0x100;
    set_carry_flag(cf > 0);
    m_emulator->elapse_cycle();
}

namespace {
opcodes::RegisterType get_register_cb(uint8_t cb_opcode) {
    // For all CB-prefixed instructions the register follows a regular pattern.
    constexpr static std::array cb_registers{opcodes::RegisterType::B,  opcodes::RegisterType::C,
                                             opcodes::RegisterType::D,  opcodes::RegisterType::E,
                                             opcodes::RegisterType::H,  opcodes::RegisterType::L,
                                             opcodes::RegisterType::HL, opcodes::RegisterType::A};
    return cb_registers[cb_opcode & 0b111];
}

constexpr std::array<opcodes::InstructionType, 8> CB_INSTRUCTION_BLOCKS{
    opcodes::InstructionType::CB_RLC,  opcodes::InstructionType::CB_RRC,
    opcodes::InstructionType::CB_RL,   opcodes::InstructionType::CB_RR,
    opcodes::InstructionType::CB_SLA,  opcodes::InstructionType::CB_SRA,
    opcodes::InstructionType::CB_SWAP, opcodes::InstructionType::CB_SRL};

opcodes::InstructionType get_instruction_cb(uint8_t cb_opcode) {
    // The last three blocks are large with 64 instructions each.
    if (cb_opcode >= 0xC0) {
        return opcodes::InstructionType::CB_SET;
    }
    if (cb_opcode >= 0x80) {
        return opcodes::InstructionType::CB_RES;
    }
    if (cb_opcode >= 0x40) {
        return opcodes::InstructionType::CB_BIT;
    }
    // The following blocks are smaller (8 blocks with 8 instructions each).
    uint8_t index = cb_opcode / 8;
    return CB_INSTRUCTION_BLOCKS[index];
}
} // namespace

void Cpu::instruction_cb_rotate_left(opcodes::RegisterType register_type) {
    bool cf = is_flag_set(flags::carry);
    auto value = cb_fetch_data(register_type);
    value = bitmanip::rotate_left_carry(value, cf);
    set_carry_flag(cf);
    set_zero_flag(value == 0);
    set_subtract_flag(BitValues::Inactive);
    set_half_carry_flag(BitValues::Inactive);
    cb_set_data(register_type, value);
}

void Cpu::instruction_cb_set_reset_bit(opcodes::InstructionType instruction_type,
                                       opcodes::RegisterType register_type, uint8_t bit) {
    auto value = cb_fetch_data(register_type);
    // Set bit instruction or Reset bit instruction. Since only the operation differs and the
    // write back stays the same, handle them accordingly.
    switch (instruction_type) {
    case opcodes::InstructionType::CB_SET:
        bitmanip::set(value, bit);
        break;
    case opcodes::InstructionType::CB_RES:
        bitmanip::unset(value, bit);
        break;
    default:
        break;
    }
    cb_set_data(register_type, value);
}

void Cpu::instruction_cb_srl(opcodes::RegisterType register_type) {
    auto value = cb_fetch_data(register_type);
    set_carry_flag((value & 1) == 1);
    value = bitmanip::rotate_right(value);
    set_zero_flag(value == 0);
    set_subtract_flag(BitValues::Inactive);
    set_half_carry_flag(BitValues::Inactive);
    cb_set_data(register_type, value);
}

void Cpu::instruction_cb_rr(opcodes::RegisterType register_type) {
    auto value = cb_fetch_data(register_type);
    auto cf = is_flag_set(flags::carry);
    value = bitmanip::rotate_right_carry(value, cf);
    set_carry_flag(cf);
    set_zero_flag(value == 0);
    set_subtract_flag(BitValues::Inactive);
    set_half_carry_flag(BitValues::Inactive);
    cb_set_data(register_type, value);
}

void Cpu::instruction_cb_swap(opcodes::RegisterType register_type) {
    auto value = cb_fetch_data(register_type);
    value = bitmanip::swap_nibbles(value);
    set_zero_flag(value == 0);
    set_subtract_flag(BitValues::Inactive);
    set_half_carry_flag(BitValues::Inactive);
    set_carry_flag(BitValues::Inactive);
    cb_set_data(register_type, value);
}

void Cpu::instruction_cb_rlc(opcodes::RegisterType register_type) {
    auto value = cb_fetch_data(register_type);
    value = instructionRLC(value);
    set_zero_flag(value == 0);
    cb_set_data(register_type, value);
}

void Cpu::instruction_cb_rrc(opcodes::RegisterType register_type) {
    auto value = cb_fetch_data(register_type);
    value = instructionRRC(value);
    set_zero_flag(value == 0);
    cb_set_data(register_type, value);
}

void Cpu::instruction_cb_sla(opcodes::RegisterType register_type) {
    auto value = cb_fetch_data(register_type);
    bool cf = false;
    value = bitmanip::rotate_left_carry(value, cf);
    set_zero_flag(value == 0);
    set_subtract_flag(BitValues::Inactive);
    set_half_carry_flag(BitValues::Inactive);
    set_carry_flag(cf);
    cb_set_data(register_type, value);
}

void Cpu::instruction_cb_sra(opcodes::RegisterType register_type) {
    auto value = cb_fetch_data(register_type);
    auto cf = bitmanip::is_bit_set(value, 7);
    value = bitmanip::rotate_right_carry(value, cf);
    set_zero_flag(value == 0);
    set_subtract_flag(BitValues::Inactive);
    set_half_carry_flag(BitValues::Inactive);
    set_carry_flag(cf);
    cb_set_data(register_type, value);
}

void Cpu::cb_set_data(opcodes::RegisterType register_type, uint8_t value) {
    if (register_type == opcodes::RegisterType::HL) {
        // Indirect access
        m_emulator->get_bus()->write_byte(registers.hl, value);
        m_emulator->elapse_cycle();
    } else {
        set_register_value(register_type, value);
    }
}

uint8_t Cpu::cb_fetch_data(opcodes::RegisterType register_type) {
    if (register_type == opcodes::RegisterType::HL) {
        // Indirect access
        auto x = m_emulator->get_bus()->read_byte(registers.hl);
        m_emulator->elapse_cycle();
        return x;
    }
    return static_cast<uint8_t>(get_register_value(register_type));
}

void Cpu::instructionCB(uint8_t cb_opcode) {
    auto instruction_type = get_instruction_cb(cb_opcode);
    auto register_type = get_register_cb(cb_opcode);
    auto bit_position = internal::op_code_to_bit(cb_opcode);
    m_logger->debug("Executing CB {:02X} {} {}", cb_opcode, magic_enum::enum_name(instruction_type),
                    magic_enum::enum_name(register_type));

    switch (instruction_type) {
    case opcodes::InstructionType::CB_RLC:
        instruction_cb_rlc(register_type);
        break;
    case opcodes::InstructionType::CB_RRC:
        instruction_cb_rrc(register_type);
        break;
    case opcodes::InstructionType::CB_SLA:
        instruction_cb_sla(register_type);
        break;
    case opcodes::InstructionType::CB_SRA:
        instruction_cb_sra(register_type);
        break;
    case opcodes::InstructionType::CB_SET:
    case opcodes::InstructionType::CB_RES:
        instruction_cb_set_reset_bit(instruction_type, register_type, bit_position);
        break;
    case opcodes::InstructionType::CB_BIT:
        instruction_cb_test_bit(register_type, bit_position);
        break;
    case opcodes::InstructionType::CB_RL:
        instruction_cb_rotate_left(register_type);
        break;
    case opcodes::InstructionType::CB_SRL:
        instruction_cb_srl(register_type);
        break;
    case opcodes::InstructionType::CB_RR:
        instruction_cb_rr(register_type);
        break;
    case opcodes::InstructionType::CB_SWAP:
        instruction_cb_swap(register_type);
        break;
    default:
        abort_execution<NotImplementedError>(fmt::format("CB instruction {} not implemented",
                                                         magic_enum::enum_name(instruction_type)));
    }
}

void Cpu::instructionJR(opcodes::Instruction instruction, uint8_t data) {
    bool jump_condition = check_condition(instruction.condition_type);
    if (!jump_condition) {
        return;
    }
    // For C++20 this is defined behaviour since signed integers are twos complement
    // On older standards this is implementation defined
    // https://stackoverflow.com/questions/13150449/efficient-unsigned-to-signed-cast-avoiding-implementation-defined-behavior
    auto immediate = static_cast<int8_t>(data);
    registers.pc += immediate;
    m_emulator->elapse_cycle();
}

bool Cpu::check_condition(opcodes::ConditionType condition_type) const {
    switch (condition_type) {
    case opcodes::ConditionType::None:
        return true;
    case opcodes::ConditionType::NonCarry:
        return !is_flag_set(flags::carry);
    case opcodes::ConditionType::NonZero:
        return !is_flag_set(flags::zero);
    case opcodes::ConditionType::Zero:
        return is_flag_set(flags::zero);
    case opcodes::ConditionType::Carry:
        return is_flag_set(flags::carry);
    }
    // GCCs Wreturn-type warns here, because it assumes programmers use the whole range of the
    // undlerlying type of the enum. Since I don't want to have a default statement, which would
    // cover adding a new entry to the enum but not handling it in the switch (which warns), I
    // tell GCC that falling out of the switch is impossible. This is the case when not casting
    // integers to the enum, which I don't do.
    // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=87951
    __builtin_unreachable();
}

void Cpu::instructionJP(opcodes::Instruction instruction, uint16_t data) {
    if (!check_condition(instruction.condition_type)) {
        return;
    }
    if (instruction.interaction_type == opcodes::InteractionType::AddressRegister) {
        data = registers.hl;
    } else {
        m_emulator->elapse_cycle();
    }
    registers.pc = data;
}

void Cpu::instructionINCDEC(opcodes::Instruction instruction) {
    uint16_t value_original{};
    // INC and DEC only differ by the operation (+/-). The timings and flags behaviour is the same.
    // We can implement both using the same function.
    auto operation = instruction.instruction_type == opcodes::InstructionType::INC
                         ? [](int a, int b) { return a + b; }
                         : [](int a, int b) { return a - b; };
    if (instruction.interaction_type == opcodes::InteractionType::AddressRegister) {
        // Indirect access
        auto address = get_register_value(instruction.register_type_destination);
        value_original = m_emulator->get_bus()->read_byte(address);
        m_emulator->elapse_cycle();
        m_emulator->get_bus()->write_byte(address, operation(value_original, 1));
        m_emulator->elapse_cycle();
    } else {
        // Direct access
        value_original = get_register_value(instruction.register_type_destination);
        set_register_value(instruction.register_type_destination, operation(value_original, 1));
    }
    // Setting flags ( The two additional cycles are for all word INC/DECs which are not indirect).
    switch (instruction.register_type_destination) {
    case opcodes::RegisterType::BC:
    case opcodes::RegisterType::DE:
    case opcodes::RegisterType::SP:
        m_emulator->elapse_cycle();
        // No flags set in case of word register operations except when HL is functioning as an
        // address register
        break;
    case opcodes::RegisterType::HL:
        if (instruction.interaction_type == opcodes::InteractionType::Register) {
            m_emulator->elapse_cycle();
            break;
        }
        [[fallthrough]];
    default:
        auto was_hc = internal::was_half_carry(value_original, 1, operation);
        set_half_carry_flag(was_hc);
        // Truncate to 8 bits to catch overflows/underflows
        set_zero_flag(static_cast<uint8_t>(operation(value_original, 1)) == 0);
        set_subtract_flag(instruction.instruction_type == opcodes::InstructionType::INC
                              ? BitValues::Inactive
                              : BitValues::Active);
    }
}

void Cpu::instructionCALL(opcodes::Instruction instruction, uint16_t data) {
    bool condition_met = check_condition(instruction.condition_type);
    if (condition_met) {
        m_emulator->elapse_cycle();
        push_word_on_stack(registers.pc);
        registers.pc = data;
    }
}
void Cpu::instructionPUSH(opcodes::Instruction instruction) {
    m_emulator->elapse_cycle();
    auto value = get_register_value(instruction.register_type_destination);
    push_word_on_stack(value);
}

void Cpu::instructionRL(opcodes::Instruction instruction) {
    if (instruction.instruction_type == opcodes::InstructionType::RL) {
        bool cf = is_flag_set(flags::carry);
        registers.a = bitmanip::rotate_left_carry(registers.a, cf);
        set_carry_flag(cf);
        set_zero_flag(BitValues::Inactive);
        set_subtract_flag(BitValues::Inactive);
        set_half_carry_flag(BitValues::Inactive);
    } else {
        abort_execution<LogicError>(fmt::format("Unexpected instruction {} in RL", instruction));
    }
}

void Cpu::instructionPOP(opcodes::Instruction instruction) {
    auto value = pop_word_from_stack();
    if (instruction.register_type_destination == opcodes::RegisterType::AF) {
        // The lower nibble of F can't be changed and is always 0, so we avoid writing it here.
        value &= 0xFFF0;
    }
    set_register_value(instruction.register_type_destination, value);
}

void Cpu::instructionRET(opcodes::Instruction instruction) {
    bool condition_met = check_condition(instruction.condition_type);
    if (instruction.condition_type != opcodes::ConditionType::None) {
        m_emulator->elapse_cycle();
    }
    if (condition_met) {
        registers.pc = pop_word_from_stack();
        m_emulator->elapse_cycle();
    }
}

void Cpu::instructionCP(opcodes::Instruction instruction, uint8_t data) {
    // For ImmediateByte interaction type data was already fetched
    if (instruction.interaction_type == opcodes::InteractionType::Register_Register) {
        data = get_register_value(instruction.register_type_source);
    } else if (instruction.interaction_type == opcodes::InteractionType::Register_AddressRegister) {
        auto address = get_register_value(instruction.register_type_source);
        data = m_emulator->get_bus()->read_byte(address);
        m_emulator->elapse_cycle();
    }
    set_zero_flag(registers.a == data);
    const auto hc = internal::was_half_carry(registers.a, data, std::minus{});
    set_half_carry_flag(hc);
    set_subtract_flag(BitValues::Active);
    set_carry_flag(registers.a < data);
}

void Cpu::instructionANDORXOR(opcodes::Instruction instruction, uint8_t data) {
    if (instruction.interaction_type == opcodes::InteractionType::Register_Register) {
        data = get_register_value(instruction.register_type_source);
    } else if (instruction.interaction_type == opcodes::InteractionType::Register_AddressRegister) {
        auto address = get_register_value(instruction.register_type_source);
        data = m_emulator->get_bus()->read_byte(address);
        m_emulator->elapse_cycle();
    }
    switch (instruction.instruction_type) {
    case opcodes::InstructionType::AND:
        registers.a &= data;
        break;
    case opcodes::InstructionType::OR:
        registers.a |= data;
        break;
    case opcodes::InstructionType::XOR:
        registers.a ^= data;
        break;
    default:
        break;
    }

    set_zero_flag(registers.a == 0);
    set_subtract_flag(BitValues::Inactive);
    if (instruction.instruction_type == opcodes::InstructionType::AND) {
        set_half_carry_flag(BitValues::Active);
    } else {
        set_half_carry_flag(BitValues::Inactive);
    }
    set_carry_flag(BitValues::Inactive);
}


void Cpu::instructionSUB(opcodes::Instruction instruction, uint8_t data) {
    // For ImmediateByte interaction type data was already fetched
    if (instruction.interaction_type == opcodes::InteractionType::Register_Register) {
        data = get_register_value(instruction.register_type_source);
    } else if (instruction.interaction_type == opcodes::InteractionType::Register_AddressRegister) {
        auto address = get_register_value(instruction.register_type_source);
        data = m_emulator->get_bus()->read_byte(address);
        m_emulator->elapse_cycle();
    }
    const auto hc = internal::was_half_carry(registers.a, data, std::minus{});
    set_carry_flag(registers.a < data);
    registers.a -= data;
    set_zero_flag(registers.a == 0);
    set_subtract_flag(BitValues::Active);
    set_half_carry_flag(hc);
}

void Cpu::instructionADD(opcodes::Instruction instruction, uint16_t data) {
    // For ImmediateByte interaction type data was already fetched
    if (instruction.interaction_type == opcodes::InteractionType::Register_Register) {
        data = get_register_value(instruction.register_type_source);
    } else if (instruction.interaction_type == opcodes::InteractionType::Register_AddressRegister) {
        auto address = get_register_value(instruction.register_type_source);
        data = m_emulator->get_bus()->read_byte(address);
        m_emulator->elapse_cycle();
    }
    auto destination_register_value = get_register_value(instruction.register_type_destination);
    const auto hc = [&] {
        if (instruction.register_type_destination == opcodes::RegisterType::HL) {
            return internal::was_half_carry_word(destination_register_value, data);
        }
        return internal::was_half_carry(destination_register_value, data, std::plus{});
    }();
    set_half_carry_flag(hc);
    const auto was_carry = [&] {
        if (instruction.register_type_destination == opcodes::RegisterType::HL) {
            return internal::was_carry_word(destination_register_value, data);
        }
        return internal::was_carry(destination_register_value, data, std::plus{});
    }();
    set_carry_flag(was_carry);
    set_subtract_flag(BitValues::Inactive);
    set_register_value(instruction.register_type_destination, destination_register_value + data);
    // The word register ADD instructions don't modify the zero flag, but they take one cycle
    // longer.
    if (instruction.register_type_destination != opcodes::RegisterType::HL) {
        set_zero_flag(get_register_value(instruction.register_type_destination) == 0);
    } else {
        m_emulator->elapse_cycle();
    }
}

void Cpu::instructionADD_Signed(int8_t data) {
    set_zero_flag(BitValues::Inactive);
    set_subtract_flag(BitValues::Inactive);
    auto result = static_cast<int>(registers.sp + data);
    set_half_carry_flag(((registers.sp ^ data ^ (result & 0xFFFF)) & 0x10) == 0x10);
    set_carry_flag(((registers.sp ^ data ^ (result & 0xFFFF)) & 0x100) == 0x100);
    registers.sp = result;
    m_emulator->elapse_cycle();
    m_emulator->elapse_cycle();
}

void Cpu::instructionRETI() {
    registers.pc = pop_word_from_stack();
    m_emulator->elapse_cycle();
    m_emulator->set_interrupts_enabled(true);
}

void Cpu::instructionADC(opcodes::Instruction instruction, uint8_t data) {
    // For ImmediateByte interaction type data was already fetched
    if (instruction.interaction_type == opcodes::InteractionType::Register_Register) {
        data = get_register_value(instruction.register_type_source);
    } else if (instruction.interaction_type == opcodes::InteractionType::Register_AddressRegister) {
        auto address = get_register_value(instruction.register_type_source);
        data = m_emulator->get_bus()->read_byte(address);
        m_emulator->elapse_cycle();
    }
    auto carry = bitmanip::bit_value(registers.f, static_cast<uint8_t>(flags::carry));
    auto sum = registers.a + data + carry;
    auto was_carry = sum > 0xFF;
    auto was_half_carry = ((registers.a & 0xF) + (data & 0xF) + carry) > 0xF;
    registers.a = sum;
    set_zero_flag(registers.a == 0);
    set_subtract_flag(BitValues::Inactive);
    set_half_carry_flag(was_half_carry);
    set_carry_flag(was_carry);
}

void Cpu::instructionSBC(opcodes::Instruction instruction, uint8_t data) {
    // For ImmediateByte interaction type data was already fetched
    if (instruction.interaction_type == opcodes::InteractionType::Register_Register) {
        data = get_register_value(instruction.register_type_source);
    } else if (instruction.interaction_type == opcodes::InteractionType::Register_AddressRegister) {
        auto address = get_register_value(instruction.register_type_source);
        data = m_emulator->get_bus()->read_byte(address);
        m_emulator->elapse_cycle();
    }
    auto carry = bitmanip::bit_value(registers.f, static_cast<uint8_t>(flags::carry));
    auto result = registers.a - data - carry;
    auto was_carry = result < 0;
    auto was_half_carry = ((registers.a & 0xF) - (data & 0xF) - carry) < 0;
    registers.a = result;
    set_zero_flag(registers.a == 0);
    set_subtract_flag(BitValues::Active);
    set_half_carry_flag(was_half_carry);
    set_carry_flag(was_carry);
}

void Cpu::instructionDAA() {
    // Thanks to https://ehaskins.com/2018-01-30%20Z80%20DAA/
    // and https://forums.nesdev.org/viewtopic.php?t=15944
    if (!is_flag_set(flags::subtract)) {
        // After an addition, adjust if (half-)carry occured or if result is out of bounds
        if (is_flag_set(flags::carry) or registers.a > 0x99) {
            registers.a += 0x60;
            set_carry_flag(BitValues::Active);
        }
        if (is_flag_set(flags::half_carry) || (registers.a & 0x0F) > 0x09) {
            registers.a += 0x06;
        }
    } else {
        // After a subtraction, only adjust if (half-)carry occured
        if (is_flag_set(flags::carry)) {
            registers.a -= 0x60;
        }
        if (is_flag_set(flags::half_carry)) {
            registers.a -= 0x6;
        }
    }
    set_zero_flag(registers.a == 0);
    set_half_carry_flag(BitValues::Inactive);
}

void Cpu::instructionRST(uint8_t opcode) {
    m_emulator->elapse_cycle();
    push_word_on_stack(registers.pc);
    auto new_address = opcode - 0xC7;
    registers.pc = new_address;
}

void Cpu::instructionCPL() {
    registers.a ^= 0xFF;
    set_subtract_flag(BitValues::Active);
    set_half_carry_flag(BitValues::Active);
}

void Cpu::instructionSCF() {
    set_subtract_flag(BitValues::Inactive);
    set_half_carry_flag(BitValues::Inactive);
    set_carry_flag(BitValues::Active);
}

void Cpu::instructionCCF() {
    set_carry_flag(!is_flag_set(flags::carry));
    set_subtract_flag(BitValues::Inactive);
    set_half_carry_flag(BitValues::Inactive);
}

uint8_t Cpu::instructionRLC(uint8_t data) {
    bool cf = bitmanip::is_bit_set(data, 7);
    set_carry_flag(cf);
    data = bitmanip::rotate_left_carry(data, cf);
    set_zero_flag(BitValues::Inactive);
    set_subtract_flag(BitValues::Inactive);
    set_half_carry_flag(BitValues::Inactive);
    return data;
}

uint8_t Cpu::instructionRRC(uint8_t data) {
    bool cf = bitmanip::is_bit_set(data, 0);
    set_carry_flag(cf);
    data = bitmanip::rotate_right_carry(data, cf);
    set_zero_flag(BitValues::Inactive);
    set_subtract_flag(BitValues::Inactive);
    set_half_carry_flag(BitValues::Inactive);
    return data;
}


void Cpu::call_isr(uint16_t isr_address) {
    m_emulator->elapse_cycle();
    m_emulator->elapse_cycle();
    push_word_on_stack(registers.pc);
    registers.pc = isr_address;
    m_emulator->elapse_cycle();
}

void Cpu::set_initial_state() {
    registers.a = 1;
    registers.f = 0xB0;
    registers.b = 0;
    registers.c = 0x13;
    registers.d = 0;
    registers.e = 0xD8;
    registers.h = 0x01;
    registers.l = 0x4D;
    registers.pc = 0x100;
    registers.sp = 0xFFFE;
}

void Cpu::push_word_on_stack(uint16_t x) {
    registers.sp--;
    auto bus = m_emulator->get_bus();
    bus->write_byte(registers.sp, bitmanip::get_high_byte(x));
    m_emulator->elapse_cycle();
    registers.sp--;
    bus->write_byte(registers.sp, bitmanip::get_low_byte(x));
    m_emulator->elapse_cycle();
}

uint16_t Cpu::pop_word_from_stack() {
    auto bus = m_emulator->get_bus();
    auto low_byte = bus->read_byte(registers.sp);
    registers.sp++;
    m_emulator->elapse_cycle();
    auto high_byte = bus->read_byte(registers.sp);
    registers.sp++;
    m_emulator->elapse_cycle();
    return bitmanip::word_from_bytes(high_byte, low_byte);
}

uint8_t internal::op_code_to_bit(uint8_t opcode_byte) {
    // Divide by lowest opcode which is regular (part of a 4x16 block in op table)
    // to handle opcodes for BIT, RES and SET instructions in the same way by projecting
    // them all to 0x00..0x3F
    opcode_byte %= 0x40;
    // higher nibble
    // |
    // | | 0 1 2 3 4 5 6 7 8 9 A B C D E F ->lower nibble
    // v |---------------------------------
    // 0x| 0 0 0 0 0 0 0 0 1 1 1 1 1 1 1 1
    // 1x| 2 2 2 2 2 2 2 2 3 3 3 3 3 3 3 3
    // 2x| 4 4 4 4 4 4 5 5 5 5 5 5 5 5 5 5
    // 3x| 6 6 6 6 6 6 7 7 7 7 7 7 7 7 7 7
    //
    // First map higher nibble 0x -> 0, 1x -> 2, 2x -> 4, 3x -> 6
    // Then add 1 if we are in the right half of the table
    return ((opcode_byte & 0xF0) >> constants::NIBBLE_SIZE) * 2
           + ((opcode_byte & 0x0F) / constants::BYTE_SIZE);
}

std::ostream& operator<<(std::ostream& os, const CpuDebugState& cds) {
    return os << fmt::format(
               "A: {:02X} F: {:02X} B: {:02X} C: {:02X} D: {:02X} E: {:02X} H: {:02X} L: "
               "{:02X} SP: {:04X} "
               "PC: {:04X} ({:02X} {:02X} {:02X} {:02X})",
               cds.a, cds.f, cds.b, cds.c, cds.d, cds.e, cds.h, cds.l, cds.sp, cds.pc,
               cds.mem_pc[0], cds.mem_pc[1], cds.mem_pc[2], cds.mem_pc[3]);
}

bool operator==(const CpuDebugState& a, const CpuDebugState& b) = default;
