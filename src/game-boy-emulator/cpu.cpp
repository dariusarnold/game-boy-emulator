#include "cpu.hpp"
#include "addressbus.hpp"
#include "bitmanipulation.hpp"
#include "emulator.hpp"
#include "exceptions.hpp"

#include <unordered_set>


bool Cpu::step() {
    previous_instruction = current_instruction;
    current_instruction = fetch_instruction();
    print(fmt::format("Executing {}\n", current_instruction), Verbosity::LEVEL_INFO);
    // TODO: the additional fetch could be done conditionally
    auto data = fetch_data(current_instruction);
    print(fmt::format("Data {:04X}\n", data), Verbosity::LEVEL_INFO);
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
    case opcodes::InstructionType::XOR:
        instructionXOR(current_instruction);
        break;
    case opcodes::InstructionType::CB:
        // Here the data is the second byte of the CB instruction.
        instructionCB(data);
        break;
    case opcodes::InstructionType::JR:
        instructionJR(current_instruction, data);
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
    case opcodes::InstructionType::SUB:
        instructionSUB(current_instruction, data);
        break;
    case opcodes::InstructionType::ADD:
        instructionADD(current_instruction, data);
        break;
    default:
        abort_execution<NotImplementedError>(
            fmt::format("Instruction type {} not implemented",
                        magic_enum::enum_name(current_instruction.instruction_type)));
    }
    instructions_executed++;
    return true;
}

void Cpu::run() {
    while (step()) {
    }
}

Cpu::Cpu(Emulator* emulator): Cpu(emulator,Verbosity::LEVEL_INFO){}

Cpu::Cpu(Emulator* emulator, Verbosity verbosity_) : m_emulator(emulator), verbosity(verbosity_) {}

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

void Cpu::test_bit(uint8_t value, u_int8_t position) {
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
    print(fmt::format("Fetched opcode {:02X}\n", byte), Verbosity::LEVEL_DEBUG);
    m_emulator->elapse_cycles(1);
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
        m_emulator->elapse_cycles(1);
        return low_byte;
    case opcodes::InteractionType::ImmediateWord:
    case opcodes::InteractionType::Register_AddressWord:
    case opcodes::InteractionType::AddressWord_Register:
        low_byte = m_emulator->get_bus()->read_byte(registers.pc);
        registers.pc++;
        m_emulator->elapse_cycles(1);
        high_byte = m_emulator->get_bus()->read_byte(registers.pc);
        registers.pc++;
        m_emulator->elapse_cycles(1);
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

void Cpu::print(std::string_view message, Verbosity level) {
    if (static_cast<int>(level) <= static_cast<int>(verbosity)) {
        fmt::print(message);
    }
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
        m_emulator->elapse_cycles(1);
        if (instruction.instruction_type == opcodes::InstructionType::LDD) {
            set_register_value(instruction.register_type_destination, value - 1);
        } else if (instruction.instruction_type == opcodes::InstructionType::LDI) {
            set_register_value(instruction.register_type_destination, value + 1);
        }
        return;
    case opcodes::InteractionType::Register_AddressRegister:
        value = m_emulator->get_bus()->read_byte(
            get_register_value(instruction.register_type_source));
        m_emulator->elapse_cycles(1);
        set_register_value(instruction.register_type_destination, value);
        if (instruction.instruction_type == opcodes::InstructionType::LDD) {
            set_register_value(instruction.register_type_source, value - 1);
        } else if (instruction.instruction_type == opcodes::InstructionType::LDI) {
            set_register_value(instruction.register_type_source, value + 1);
        }
        return;
    case opcodes::InteractionType::AddressRegister_ImmediateByte:
        // read address register
        value = get_register_value(instruction.register_type_destination);
        m_emulator->get_bus()->write_byte(value, data);
        m_emulator->elapse_cycles(1);
        return;
    case opcodes::InteractionType::AddressWord_Register:
        m_emulator->get_bus()->write_byte(data,
                                          get_register_value(instruction.register_type_source));
        m_emulator->elapse_cycles(1);
        return;
    case opcodes::InteractionType::Register_AddressWord:
        value = m_emulator->get_bus()->read_byte(data);
        m_emulator->elapse_cycles(1);
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
            m_emulator->elapse_cycles(1);
        } else {
            // 0xF0 LDH A,(n)
            registers.a = m_emulator->get_bus()->read_byte(data + offset);
            m_emulator->elapse_cycles(1);
        }
    } else {
        // 0xF2 LDH (C),A
        m_emulator->get_bus()->write_byte(registers.c + offset, registers.a);
        m_emulator->elapse_cycles(1);
    }
}

void Cpu::instructionXOR(opcodes::Instruction instruction) {
    registers.a = registers.a ^ get_register_value(instruction.register_type_source);
    if (registers.a == 0) {
        set_zero_flag(BitValues::Active);
    }
    set_subtract_flag(BitValues::Inactive);
    set_half_carry_flag(BitValues::Inactive);
    set_carry_flag(BitValues::Inactive);
}

opcodes::RegisterType get_register_cb(uint8_t cb_opcode) {
    // For all CB-prefixed instructions the register follows a regular pattern.
    constexpr static opcodes::RegisterType cb_registers[]
        = {opcodes::RegisterType::B,  opcodes::RegisterType::C, opcodes::RegisterType::D,
           opcodes::RegisterType::E,  opcodes::RegisterType::H, opcodes::RegisterType::L,
           opcodes::RegisterType::HL, opcodes::RegisterType::A};
    return cb_registers[cb_opcode & 0b111];
}

void Cpu::instructionCB(uint8_t cb_opcode) {
    auto register_type = get_register_cb(cb_opcode);
    auto bit = internal::op_code_to_bit(cb_opcode);
    uint8_t value = [&] {
        if (register_type == opcodes::RegisterType::HL) {
            // Indirect access
            auto x = m_emulator->get_bus()->read_byte(registers.hl);
            m_emulator->elapse_cycles(1);
            return x;
        } else {
            return static_cast<uint8_t>(get_register_value(register_type));
        }
    }();
    if (cb_opcode >= 0xC0) {
        // Set bit instruction or Reset bit instruction. Since only the operation differs and the
        // write back stays the same, handle them accordingly.
        if (cb_opcode >= 0x80) {
            print(fmt::format("Prefix CB Reset bit {} in {}\n", bit,
                              magic_enum::enum_name(register_type)),
                  Verbosity::LEVEL_INFO);
            bitmanip::unset(value, bit);
        } else {
            print(fmt::format("Prefix CB Set bit {} in {}\n", bit,
                              magic_enum::enum_name(register_type)),
                  Verbosity::LEVEL_INFO);
            bitmanip::set(value, bit);
        }
        if (register_type == opcodes::RegisterType::HL) {
            // Indirect access
            m_emulator->get_bus()->write_byte(registers.hl, value);
            m_emulator->elapse_cycles(1);
        } else {
            set_register_value(register_type, value);
        }
    } else if (cb_opcode >= 0x40) {
        // Bit test instruction (This just sets flags and doesn't modify registers or memory)
        print(
            fmt::format("Prefix CB Test bit {} in {}\n", bit, magic_enum::enum_name(register_type)),
            Verbosity::LEVEL_INFO);
        test_bit(value, bit);
    } else if (cb_opcode >= 0x10 && cb_opcode <= 0x17) {
        // Rotate left instruction
        auto register_ = get_register_cb(cb_opcode);
        bool cf = is_flag_set(flags::carry);
        value = get_register_value(register_);
        set_register_value(register_, bitmanip::rotate_left_carry(value, cf));
        set_carry_flag(cf);
        set_zero_flag(get_register_value(register_) == 0);
        set_subtract_flag(BitValues::Inactive);
        set_half_carry_flag(BitValues::Inactive);
    } else {
        abort_execution<NotImplementedError>(
            fmt::format("CB suffix {:02X} not implemented", cb_opcode));
    }
}

void Cpu::instructionJR(opcodes::Instruction instruction, uint8_t data) {
    bool jump_condition = [&] {
        switch (instruction.condition_type) {
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
    }();
    if (!jump_condition) {
        return;
    }
    // For C++20 this is defined behaviour since signed integers are twos complement
    // On older standards this is implementation defined
    // https://stackoverflow.com/questions/13150449/efficient-unsigned-to-signed-cast-avoiding-implementation-defined-behavior
    auto immediate = static_cast<int8_t>(data);
    registers.pc += immediate;
    m_emulator->elapse_cycles(1);
}


void Cpu::instructionINCDEC(opcodes::Instruction instruction) {
    uint16_t value_original;
    // INC and DEC only differ by the operation (+/-). The timings and flags behaviour is the same.
    // We can implement both using the same function.
    auto operation = instruction.instruction_type == opcodes::InstructionType::INC ? [](int a, int b) {return a + b;} :[](int a, int b) {return a - b;};
    if (instruction.interaction_type == opcodes::InteractionType::AddressRegister) {
        // Indirect access
        auto address = get_register_value(instruction.register_type_destination);
        value_original = m_emulator->get_bus()->read_byte(address);
        m_emulator->get_bus()->write_byte(address, operation(value_original, 1));
        m_emulator->elapse_cycles(2);
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
        set_zero_flag(operation(value_original, 1) == 0);
        set_subtract_flag(instruction.instruction_type == opcodes::InstructionType::INC ? BitValues::Inactive : BitValues::Active);
    }
}

void Cpu::instructionCALL(opcodes::Instruction instruction, uint16_t data) {
    bool condition_met = [&] {
        switch (instruction.condition_type) {
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
        // See comment in instructionJR.
        __builtin_unreachable();
    }();
    if (condition_met) {
        m_emulator->elapse_cycles(1);
        auto bus = m_emulator->get_bus();
        bus->write_byte(--registers.sp, bitmanip::get_high_byte(registers.pc));
        bus->write_byte(--registers.sp, bitmanip::get_low_byte(registers.pc));
        m_emulator->elapse_cycles(3);
        registers.pc = data;
    }
}
void Cpu::instructionPUSH(opcodes::Instruction instruction) {
    m_emulator->elapse_cycles(1);
    auto bus = m_emulator->get_bus();
    auto value = get_register_value(instruction.register_type_destination);
    registers.sp -= 1;
    bus->write_byte(registers.sp, bitmanip::get_high_byte(value));
    registers.sp -= 1;
    bus->write_byte(registers.sp, bitmanip::get_low_byte(value));
    m_emulator->elapse_cycles(2);
}

void Cpu::instructionRL(opcodes::Instruction instruction) {
    if (instruction.instruction_type == opcodes::InstructionType::RL) {
        bool cf = is_flag_set(flags::carry);
        registers.a =  bitmanip::rotate_left_carry(registers.a, cf);
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
    m_emulator->elapse_cycles(1);
    auto high_byte = m_emulator->get_bus()->read_byte(registers.sp);
    registers.sp++;
    m_emulator->elapse_cycles(1);
    auto value = bitmanip::word_from_bytes(high_byte, low_byte);
    set_register_value(instruction.register_type_destination, value);
}

void Cpu::instructionRET(opcodes::Instruction instruction) {
    bool condition_met = [&] {
        switch (instruction.condition_type) {
        case opcodes::ConditionType::None:
            return true;
        case opcodes::ConditionType::NonCarry:
            m_emulator->elapse_cycles(1);
            return !is_flag_set(flags::carry);
        case opcodes::ConditionType::NonZero:
            m_emulator->elapse_cycles(1);
            return !is_flag_set(flags::zero);
        case opcodes::ConditionType::Zero:
            m_emulator->elapse_cycles(1);
            return is_flag_set(flags::zero);
        case opcodes::ConditionType::Carry:
            m_emulator->elapse_cycles(1);
            return is_flag_set(flags::carry);
        }
        // See comment in instructionJR.
        __builtin_unreachable();
    }();
    if (condition_met) {
        auto bus = m_emulator->get_bus();
        auto low_byte = bus->read_byte(registers.sp++);
        auto high_byte = bus->read_byte(registers.sp++);
        m_emulator->elapse_cycles(3);
        registers.pc = bitmanip::word_from_bytes(high_byte, low_byte);
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
