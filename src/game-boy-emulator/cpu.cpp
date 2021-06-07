#include "cpu.hpp"
#include "bitmanipulation.hpp"


bool Cpu::step() {
    uint8_t opcode = ram.mem[registers.pc];
    auto instruction = instructions.find(opcode);
    if (instruction == instructions.end()) {
        fmt::print("Encountered unsupported opcode {:02x}\n", opcode);
        return false;
    }
    fmt::print("Executing {:02x}\n", opcode);
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


void Cpu::set_zero_flag() {
    bitmanip::set(registers.f, as_integral(registers::flags::zero));
}

void Cpu::clear_zero_flag() {
    bitmanip::unset(registers.f, as_integral(registers::flags::zero));
}

void Cpu::xor_x(uint8_t value) {
    registers.a ^= value;
    if (registers.a == 0) {
        set_zero_flag();
    }
    registers.pc++;
}

Cpu::Cpu() {
    instructions[opcodes::NOP] = [&]() {
        this->registers.pc++;
        return 4;
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

    instructions[opcodes::INC_BC] = [&]() {this->inc16(this->registers.bc); return 8;};
    instructions[opcodes::INC_DE] = [&]() {this->inc16(this->registers.de); return 8;};
    instructions[opcodes::INC_HL] = [&]() {this->inc16(this->registers.hl); return 8;};
    instructions[opcodes::INC_SP] = [&]() {this->inc16(this->registers.sp); return 8;};

    instructions[opcodes::INC_HL_INDIRECT] = [&]() { this->inc8(this->read_memory(this->registers.hl)); return 12;};

    instructions[opcodes::LDD_HL_A] = [&]() {
        this->ldd_hl();
        return 8;
    };
    instructions[opcodes::LD_BC_NN] = [&]() {
        this->ld_x(registers.bc);
        return 12;
    };
    instructions[opcodes::LD_DE_NN] = [&]() {
        this->ld_x(registers.de);
        return 12;
    };
    instructions[opcodes::LD_HL_NN] = [&]() {
        this->ld_x(registers.hl);
        return 12;
    };
    instructions[opcodes::LD_SP_NN] = [&]() {
        this->ld_x(registers.sp);
        return 12;
    };
    instructions[opcodes::XOR_A] = [&]() {
        this->xor_x(this->registers.a);
        return 4;
    };
    instructions[opcodes::XOR_B] = [&]() {
        this->xor_x(this->registers.b);
        return 4;
    };
    instructions[opcodes::XOR_C] = [&]() {
        this->xor_x(this->registers.c);
        return 4;
    };
    instructions[opcodes::XOR_D] = [&]() {
        this->xor_x(this->registers.d);
        return 4;
    };
    instructions[opcodes::XOR_E] = [&]() {
        this->xor_x(this->registers.e);
        return 4;
    };
    instructions[opcodes::XOR_H] = [&]() {
        this->xor_x(this->registers.h);
        return 4;
    };
    instructions[opcodes::XOR_L] = [&]() {
        this->xor_x(this->registers.l);
        return 4;
    };
    instructions[opcodes::XOR_HL] = [&]() {
        this->xor_x(this->read_memory(this->registers.hl));
        return 8;
    };
    instructions[opcodes::XOR_N] = [&]() {
        this->xor_x(this->registers.a);
        return 8;
    };
}

uint8_t Cpu::read_memory(u_int16_t address) const {
    return ram.mem[address];
}

uint8_t& Cpu::read_memory(u_int16_t address) {
    return ram.mem[address];;
}

void Cpu::write_memory(uint16_t address, uint8_t value) {
    ram.mem[address] = value;
}

void Cpu::ld_x(uint16_t& input) {
    registers.pc++;
    // read low byte
    uint16_t data = 0;
    data += ram.mem[registers.pc];
    // read high byte
    registers.pc++;
    data += ram.mem[registers.pc] << 8;
    input = data;
    registers.pc++;
}

void Cpu::ldd_hl() {
    write_memory(registers.hl, registers.a);
    registers.hl--;
    registers.pc++;
}

void Cpu::inc8(uint8_t& input) {
    input++;
    if (input == 0) set_zero_flag();
    registers.pc++;
}

void Cpu::inc16(uint16_t& input) {
    input++;
    if (input == 0) set_subtract_flag(BitValues::Active);
    registers.pc++;
}

void Cpu::set_subtract_flag(BitValues value) {
    bitmanip::set(registers.f, as_integral(registers::flags::subtract), value);
}
