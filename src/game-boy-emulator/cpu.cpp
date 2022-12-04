#include "cpu.hpp"
#include "addressbus.hpp"
#include "bitmanipulation.hpp"
#include "emulator.hpp"
#include "exceptions.hpp"

#include "spdlog/spdlog.h"


bool Cpu::step() {
    previous_instruction = current_instruction;
    current_instruction = fetch_instruction();
    m_logger->debug("Executing {}", current_instruction);
    // TODO: the additional fetch could be done conditionally
    auto data = fetch_data(current_instruction);
    m_logger->info("Data {:04X}", data);
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
        registers.hl = registers.sp + data;
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
    case opcodes::InstructionType::NOP:
        m_emulator->elapse_cycle();
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
    default:
        abort_execution<NotImplementedError>(
            fmt::format("Instruction type {} not implemented",
                        magic_enum::enum_name(current_instruction.instruction_type)));
    }
    m_emulator->elapse_instruction();
    return true;
}

void Cpu::run() {
    while (step()) {
    }
}

Cpu::Cpu(Emulator* emulator) : m_emulator(emulator), m_logger(spdlog::get("")) {}

void Cpu::set_subtract_flag(BitValues value) {
    bitmanip::set(registers.f, as_integral(flags::subtract), value);
}

void Cpu::set_half_carry_flag(BitValues value) {
    bitmanip::set(registers.f, as_integral(flags::half_carry), value);
}

void Cpu::set_zero_flag(BitValues value) {
    bitmanip::set(registers.f, as_integral(flags::zero), value);
}

void Cpu::set_carry_flag(BitValues value) {
    bitmanip::set(registers.f, as_integral(flags::carry), value);
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

uint8_t Cpu::op_code_to_register(uint8_t opcode_byte) {
    switch (opcode_byte % 8) {
    case 0:
        return registers.b;
    case 1:
        return registers.c;
    case 2:
        return registers.d;
    case 3:
        return registers.e;
    case 4:
        return registers.h;
    case 5:
        return registers.l;
    case 6:
        return m_emulator->get_bus()->read_byte(registers.hl);
    case 7:
        return registers.a;
    default:
        throw std::logic_error(fmt::format("Wrong register for opcode {:02x}", opcode_byte));
    }
}

void Cpu::write_to_destination(uint8_t opcode_byte, uint8_t value) {
    switch (opcode_byte % 8) {
    case 0:
        registers.b = value;
        break;
    case 1:
        registers.c = value;
        break;
    case 2:
        registers.d = value;
        break;
    case 3:
        registers.e = value;
        break;
    case 4:
        registers.h = value;
        break;
    case 5:
        registers.l = value;
        break;
    case 6:
        m_emulator->get_bus()->write_byte(registers.hl, value);
        break;
    case 7:
        registers.a = value;
        break;
    default:
        throw std::logic_error(fmt::format("Wrong register for opcode {:02x}", opcode_byte));
    }
}

/**
 * Check if second byte of CB opcode did indirect memory access
 * @return True if RAM was accessed, else false
 */
bool was_indirect_access(uint8_t opcode_byte) {
    return opcode_byte % 8 == 6;
}

opcodes::Instruction Cpu::fetch_instruction() {
    auto byte = m_emulator->get_bus()->read_byte(registers.pc);
    auto instruction = opcodes::get_instruction_by_value(byte);
    if (instruction.instruction_type == opcodes::InstructionType::NONE) {
        abort_execution<NotImplementedError>(
            fmt::format("Encountered unsupported opcode {:02X} at pc {:04X}", byte, registers.pc));
    }
    registers.pc++;
    m_logger->info("Fetched opcode {:02X}", byte);
    m_emulator->elapse_cycle();
    return instruction;
}

uint16_t Cpu::fetch_data(opcodes::Instruction instruction) {
    uint8_t high_byte = 0;
    uint8_t low_byte = 0;
    switch (instruction.interaction_type) {
    case opcodes::InteractionType::None:
    case opcodes::InteractionType::Register:
    case opcodes::InteractionType::AddressRegister:
    case opcodes::InteractionType::Register_Register:
    case opcodes::InteractionType::AddressRegister_Register:
    case opcodes::InteractionType::Register_AddressRegister:
        return 0;
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

opcodes::Instruction Cpu::get_current_instruction() {
    return current_instruction;
}

opcodes::Instruction Cpu::get_previous_instruction() {
    return previous_instruction;
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
        value = get_register_value(instruction.register_type_source);
        set_register_value(instruction.register_type_destination, value);
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
        m_emulator->get_bus()->write_byte(data,
                                          get_register_value(instruction.register_type_source));
        m_emulator->elapse_cycle();
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
    if (instruction.interaction_type == opcodes::InteractionType::ImmediateByte) {
        if (instruction.register_type_destination == opcodes::RegisterType::None) {
            // 0xE0 LDH (n),A data is the address where we have to store A
            m_emulator->get_bus()->write_byte(data + offset, registers.a);
            m_emulator->elapse_cycle();
        } else {
            // 0xF0 LDH A,(n)
            registers.a = m_emulator->get_bus()->read_byte(data + offset);
            m_emulator->elapse_cycle();
        }
    } else {
        // 0xF2 LDH (C),A
        m_emulator->get_bus()->write_byte(registers.c + offset, registers.a);
        m_emulator->elapse_cycle();
    }
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
    m_logger->debug("Executing {} {}", magic_enum::enum_name(instruction_type),
                    magic_enum::enum_name(register_type));

    switch (instruction_type) {
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
        data = registers.sp;
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
    switch (instruction.register_type_destination) {
    case opcodes::RegisterType::BC:
    case opcodes::RegisterType::DE:
    case opcodes::RegisterType::HL:
    case opcodes::RegisterType::SP:
        // No flags set in case of word register operations
        break;
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
        auto bus = m_emulator->get_bus();
        bus->write_byte(--registers.sp, bitmanip::get_high_byte(registers.pc));
        m_emulator->elapse_cycle();
        bus->write_byte(--registers.sp, bitmanip::get_low_byte(registers.pc));
        m_emulator->elapse_cycle();
        registers.pc = data;
        m_emulator->elapse_cycle();
    }
}
void Cpu::instructionPUSH(opcodes::Instruction instruction) {
    m_emulator->elapse_cycle();
    auto bus = m_emulator->get_bus();
    auto value = get_register_value(instruction.register_type_destination);
    registers.sp -= 1;
    bus->write_byte(registers.sp, bitmanip::get_high_byte(value));
    m_emulator->elapse_cycle();
    registers.sp -= 1;
    bus->write_byte(registers.sp, bitmanip::get_low_byte(value));
    m_emulator->elapse_cycle();
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
    auto low_byte = m_emulator->get_bus()->read_byte(registers.sp);
    registers.sp++;
    m_emulator->elapse_cycle();
    auto high_byte = m_emulator->get_bus()->read_byte(registers.sp);
    registers.sp++;
    m_emulator->elapse_cycle();
    auto value = bitmanip::word_from_bytes(high_byte, low_byte);
    set_register_value(instruction.register_type_destination, value);
}

void Cpu::instructionRET(opcodes::Instruction instruction) {
    bool condition_met = check_condition(instruction.condition_type);
    if (instruction.condition_type != opcodes::ConditionType::None) {
        m_emulator->elapse_cycle();
    }
    if (condition_met) {
        auto bus = m_emulator->get_bus();
        auto low_byte = bus->read_byte(registers.sp++);
        m_emulator->elapse_cycle();
        auto high_byte = bus->read_byte(registers.sp++);
        m_emulator->elapse_cycle();
        registers.pc = bitmanip::word_from_bytes(high_byte, low_byte);
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
    }
    const auto hc = internal::was_half_carry(registers.a, data, std::minus{});
    set_carry_flag(registers.a < data);
    registers.a -= data;
    set_zero_flag(registers.a == 0);
    set_subtract_flag(BitValues::Active);
    set_half_carry_flag(hc);
}

void Cpu::instructionADD(opcodes::Instruction instruction, uint8_t data) {
    // For ImmediateByte interaction type data was already fetched
    if (instruction.interaction_type == opcodes::InteractionType::Register_Register) {
        data = get_register_value(instruction.register_type_source);
    } else if (instruction.interaction_type == opcodes::InteractionType::Register_AddressRegister) {
        auto address = get_register_value(instruction.register_type_source);
        data = m_emulator->get_bus()->read_byte(address);
    }
    const auto hc = internal::was_half_carry(registers.a, data, std::plus{});
    set_half_carry_flag(hc);
    const auto was_carry = internal::was_carry(registers.a, data, std::plus{});
    set_carry_flag(was_carry);
    set_subtract_flag(BitValues::Inactive);
    registers.a += data;
    set_zero_flag(registers.a == 0);
}

void Cpu::instructionRETI() {
    auto bus = m_emulator->get_bus();
    auto low_byte = bus->read_byte(registers.sp++);
    m_emulator->elapse_cycle();
    auto high_byte = bus->read_byte(registers.sp++);
    m_emulator->elapse_cycle();
    registers.pc = bitmanip::word_from_bytes(high_byte, low_byte);
    m_emulator->elapse_cycle();
    m_emulator->set_interrupts_enabled(true);
}

void Cpu::call_isr(uint16_t isr_address) {
    m_emulator->elapse_cycle();
    m_emulator->elapse_cycle();
    auto bus = m_emulator->get_bus();
    registers.sp -= 1;
    bus->write_byte(registers.sp, bitmanip::get_high_byte(registers.pc));
    m_emulator->elapse_cycle();
    registers.sp -= 1;
    bus->write_byte(registers.sp, bitmanip::get_low_byte(registers.pc));
    m_emulator->elapse_cycle();
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
