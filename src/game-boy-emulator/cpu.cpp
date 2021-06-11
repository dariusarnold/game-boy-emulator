#include "cpu.hpp"
#include "bitmanipulation.hpp"


bool Cpu::step() {
    auto opcode = opcodes::OpCode{read_memory(registers.pc)};
    auto instruction = instructions.find(opcode);
    if (instruction == instructions.end()) {
        fmt::print("Encountered unsupported opcode {:02x}\n", opcode.value);
        return false;
    }
    fmt::print("Executing {:02x}\n", opcode.value);
    cycles += instruction->second();
    return true;
}

void Cpu::set_boot_rom(const std::array<uint8_t, constants::BOOT_ROM_SIZE>& boot_rom) {
    std::copy(boot_rom.begin(), boot_rom.end(), ram.mem.begin());
}

void Cpu::run() {
    print_registers();
    while (step()) {
        //        print_registers();
    }
}

void Cpu::print_registers() const {
    fmt::print("{}\n", registers);
}

void Cpu::xor8(uint8_t value) {
    registers.a ^= value;
    if (registers.a == 0) {
        set_zero_flag(BitValues::Active);
    }
    set_subtract_flag(BitValues::Inactive);
    set_half_carry_flag(BitValues::Inactive);
    set_carry_flag(BitValues::Inactive);
    registers.pc++;
}

Cpu::Cpu() {
    instructions[opcodes::NOP] = [&]() {
        this->registers.pc++;
        return 4;
    };

    instructions[opcodes::CB] = [&]() {
        this->registers.pc++;
        return this->cb(opcodes::OpCode{read_memory(registers.pc)});
    };

    instructions[opcodes::INC_A] = [&]() {
        this->inc8(this->registers.a);
        return 4;
    };
    instructions[opcodes::INC_B] = [&]() {
        this->inc8(this->registers.b);
        return 4;
    };
    instructions[opcodes::INC_C] = [&]() {
        this->inc8(this->registers.c);
        return 4;
    };
    instructions[opcodes::INC_D] = [&]() {
        this->inc8(this->registers.d);
        return 4;
    };
    instructions[opcodes::INC_E] = [&]() {
        this->inc8(this->registers.e);
        return 4;
    };
    instructions[opcodes::INC_H] = [&]() {
        this->inc8(this->registers.h);
        return 4;
    };
    instructions[opcodes::INC_L] = [&]() {
        this->inc8(this->registers.l);
        return 4;
    };

    instructions[opcodes::INC_BC] = [&]() {
        this->inc16(this->registers.bc);
        return 8;
    };
    instructions[opcodes::INC_DE] = [&]() {
        this->inc16(this->registers.de);
        return 8;
    };
    instructions[opcodes::INC_HL] = [&]() {
        this->inc16(this->registers.hl);
        return 8;
    };
    instructions[opcodes::INC_SP] = [&]() {
        this->inc16(this->registers.sp);
        return 8;
    };

    instructions[opcodes::INC_HL_INDIRECT] = [&]() {
        this->inc8(this->read_memory(this->registers.hl));
        return 12;
    };

    instructions[opcodes::LDD_HL_A] = [&]() {
        this->ldd_hl();
        return 8;
    };
    instructions[opcodes::LD_B_N] = [&]() {
        this->ld8(registers.b);
        return 8;
    };
    instructions[opcodes::LD_C_N] = [&]() {
      this->ld8(registers.c);
      return 8;
    };
    instructions[opcodes::LD_D_N] = [&]() {
      this->ld8(registers.d);
      return 8;
    };
    instructions[opcodes::LD_E_N] = [&]() {
      this->ld8(registers.e);
      return 8;
    };
    instructions[opcodes::LD_A_N] = [&]() {
      this->ld8(registers.a);
      return 8;
    };
    instructions[opcodes::LD_L_N] = [&]() {
      this->ld8(registers.l);
      return 8;
    };
    instructions[opcodes::LD_H_N] = [&]() {
      this->ld8(registers.h);
      return 8;
    };
    instructions[opcodes::LD_BC_NN] = [&]() {
        this->ld16(registers.bc);
        return 12;
    };
    instructions[opcodes::LD_DE_NN] = [&]() {
        this->ld16(registers.de);
        return 12;
    };
    instructions[opcodes::LD_HL_NN] = [&]() {
        this->ld16(registers.hl);
        return 12;
    };
    instructions[opcodes::LD_SP_NN] = [&]() {
        this->ld16(registers.sp);
        return 12;
    };
    instructions[opcodes::XOR_A] = [&]() {
        this->xor8(this->registers.a);
        return 4;
    };
    instructions[opcodes::XOR_B] = [&]() {
        this->xor8(this->registers.b);
        return 4;
    };
    instructions[opcodes::XOR_C] = [&]() {
        this->xor8(this->registers.c);
        return 4;
    };
    instructions[opcodes::XOR_D] = [&]() {
        this->xor8(this->registers.d);
        return 4;
    };
    instructions[opcodes::XOR_E] = [&]() {
        this->xor8(this->registers.e);
        return 4;
    };
    instructions[opcodes::XOR_H] = [&]() {
        this->xor8(this->registers.h);
        return 4;
    };
    instructions[opcodes::XOR_L] = [&]() {
        this->xor8(this->registers.l);
        return 4;
    };
    instructions[opcodes::XOR_HL] = [&]() {
        this->xor8(this->read_memory(this->registers.hl));
        return 8;
    };
    instructions[opcodes::XOR_N] = [&]() {
        this->xor8(this->registers.a);
        return 8;
    };

    instructions[opcodes::JR_C] = [&] {
        this->jump_r(is_flag_set(registers::flags::carry));
        return 8;
    };
    instructions[opcodes::JR_NC] = [&] {
      this->jump_r(!is_flag_set(registers::flags::carry));
      return 8;
    };
    instructions[opcodes::JR_Z] = [&] {
      this->jump_r(is_flag_set(registers::flags::zero));
      return 8;
    };
    instructions[opcodes::JR_NZ] = [&] {
      this->jump_r(!is_flag_set(registers::flags::zero));
      return 8;
    };
}

uint8_t Cpu::read_memory(u_int16_t address) const {
    return ram.mem[address];
}

uint8_t& Cpu::read_memory(u_int16_t address) {
    return ram.mem[address];
}

void Cpu::write_memory(uint16_t address, uint8_t value) {
    ram.mem[address] = value;
}

void Cpu::ld16(uint16_t& input) {
    registers.pc++;
    // read low byte
    uint16_t data = 0;
    data += read_memory(registers.pc);
    // read high byte
    registers.pc++;
    data += read_memory(registers.pc) << 8;
    input = data;
    registers.pc++;
}

void Cpu::ld8(uint8_t& input) {
    registers.pc++;
    const auto immediate = read_memory(registers.pc);
    input = immediate;
    registers.pc++;
}

void Cpu::ldd_hl() {
    write_memory(registers.hl, registers.a);
    registers.hl--;
    registers.pc++;
}

void Cpu::inc8(uint8_t& input) {
    bool bit_3_before = bitmanip::is_bit_set(input, 3);
    input++;
    bool bit_3_after = bitmanip::is_bit_set(input, 3);
    if (bit_3_after != bit_3_before) {
        set_half_carry_flag(BitValues::Active);
    } else {
        set_half_carry_flag(BitValues::Inactive);
    }
    if (input == 0) set_zero_flag(BitValues::Active);
    set_subtract_flag(BitValues::Inactive);
    registers.pc++;
}

void Cpu::inc16(uint16_t& input) {
    bool bit_11_before = bitmanip::is_bit_set(input, 3);
    input++;
    bool bit_11_after = bitmanip::is_bit_set(input, 3);
    if (bit_11_after != bit_11_before) {
        set_half_carry_flag(BitValues::Active);
    } else {
        set_half_carry_flag(BitValues::Inactive);
    }
    set_subtract_flag(BitValues::Inactive);
    if (input == 0) set_zero_flag(BitValues::Active);
    registers.pc++;
}

void Cpu::set_subtract_flag(BitValues value) {
    bitmanip::set(registers.f, as_integral(registers::flags::subtract), value);
}

void Cpu::set_half_carry_flag(BitValues value) {
    bitmanip::set(registers.f, as_integral(registers::flags::half_carry), value);
}

void Cpu::set_zero_flag(BitValues value) {
    bitmanip::set(registers.f, as_integral(registers::flags::zero), value);
}

void Cpu::set_carry_flag(BitValues value) {
    bitmanip::set(registers.f, as_integral(registers::flags::carry), value);
}

bool Cpu::is_flag_set(registers::flags flag) const {
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


void Cpu::reset_bit(uint8_t& value, uint8_t position) {
    bitmanip::unset(value, position);
}

void Cpu::set_bit(uint8_t& value, uint8_t position) {
    bitmanip::set(value, position);
}

uint8_t& Cpu::op_code_to_register(opcodes::OpCode opcode) {
    switch (opcode.value % 8) {
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
        return read_memory(registers.hl);
    case 7:
        return registers.a;
    default:
        throw std::logic_error(fmt::format("Wrong register for opcode {:02x}", opcode.value));
    }
}

/**
 * Check if second byte of CB opcode did indirect memory access
 * @return True if RAM was accessed, else false
 */
bool was_indirect_access(opcodes::OpCode opcode) {
    return opcode.value % 8 == 6;
}

uint8_t Cpu::cb(opcodes::OpCode op_code) {
    if (op_code.value >= opcodes::BIT_0B.value && op_code.value <= opcodes::BIT_7A.value) {
        // Set bit instruction
        test_bit(op_code_to_register(op_code), internal::op_code_to_bit(op_code));
        registers.pc++;
        if (was_indirect_access(op_code)) {
            return 12;
        }
        return 4;
    } else if (op_code.value >= opcodes::RES_0B.value && op_code.value <= opcodes::RES_7A.value) {
        // Reset bit instruction
        reset_bit(op_code_to_register(op_code), internal::op_code_to_bit(op_code));
        registers.pc++;
        return 4;
    } else if (op_code.value >= opcodes::SET_0B.value && op_code.value <= opcodes::SET_7A.value) {
        // Set bit instruction
        set_bit(op_code_to_register(op_code), internal::op_code_to_bit(op_code));
        registers.pc++;
        return 4;
    }
    // TODO implement further CB instructions
    return 0;
}

void Cpu::jump_r(bool condition_met) {
    registers.pc++;
    if (condition_met) {
        // For C++20 this is defined behaviour since signed integers are twos complement
        // On older standards this is implementation defined
        // https://stackoverflow.com/questions/13150449/efficient-unsigned-to-signed-cast-avoiding-implementation-defined-behavior
        auto immediate = static_cast<int8_t>(read_memory(registers.pc));
        // We need to increment here, otherwise the jump is off by one address
        registers.pc++;
        registers.pc += immediate;
    } else {
        // Set to instruction after immediate data if we didnt jump
        registers.pc++;
    }
}

uint8_t internal::op_code_to_bit(opcodes::OpCode opcode) {
    // Divide by lowest opcode which is regular (part of a 4x16 block in op table)
    // to handle opcodes for BIT, RES and SET instructions in the same way by projecting
    // them all to 0x00..0x3F
    opcode.value %= 0x40;
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
    return ((opcode.value & 0xF0) >> constants::NIBBLE_SIZE) * 2 +
           ((opcode.value & 0x0F) / constants::BYTE_SIZE);
}
