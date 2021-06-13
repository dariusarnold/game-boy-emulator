#include "cpu.hpp"
#include "bitmanipulation.hpp"
#include "instructions/callintermediary.hpp"
#include "instructions/copyregister.hpp"
#include "instructions/increment.hpp"


bool Cpu::step() {
    auto opcode = opcodes::OpCode{mmu.read_memory(registers.pc)};
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
    mmu.map_boot_rom(boot_rom);
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
        return this->cb(opcodes::OpCode{mmu.read_memory(registers.pc)});
    };

    instructions[opcodes::INC_A] = [&]() {
        Increment<A> ib{make_mutable_register<A>(), make_mutable_flag<flags::zero>(),
                        make_mutable_flag<flags::subtract>(),
                        make_mutable_flag<flags::half_carry>(), make_pc_incrementer()};
        return ib.execute();
    };
    instructions[opcodes::INC_B] = [&]() {
        Increment<B> ib{make_mutable_register<B>(), make_mutable_flag<flags::zero>(),
                        make_mutable_flag<flags::subtract>(),
                        make_mutable_flag<flags::half_carry>(), make_pc_incrementer()};
        return ib.execute();
    };
    instructions[opcodes::INC_C] = [&]() {
        Increment<C> ib{make_mutable_register<C>(), make_mutable_flag<flags::zero>(),
                        make_mutable_flag<flags::subtract>(),
                        make_mutable_flag<flags::half_carry>(), make_pc_incrementer()};
        return ib.execute();
    };
    instructions[opcodes::INC_D] = [&]() {
        Increment<D> ib{make_mutable_register<D>(), make_mutable_flag<flags::zero>(),
                        make_mutable_flag<flags::subtract>(),
                        make_mutable_flag<flags::half_carry>(), make_pc_incrementer()};
        return ib.execute();
    };
    instructions[opcodes::INC_E] = [&]() {
        Increment<E> ib{make_mutable_register<E>(), make_mutable_flag<flags::zero>(),
                        make_mutable_flag<flags::subtract>(),
                        make_mutable_flag<flags::half_carry>(), make_pc_incrementer()};
        return ib.execute();
    };
    instructions[opcodes::INC_H] = [&]() {
        Increment<H> ib{make_mutable_register<H>(), make_mutable_flag<flags::zero>(),
                        make_mutable_flag<flags::subtract>(),
                        make_mutable_flag<flags::half_carry>(), make_pc_incrementer()};
        return ib.execute();
    };
    instructions[opcodes::INC_L] = [&]() {
        Increment<L> ib{make_mutable_register<L>(), make_mutable_flag<flags::zero>(),
                        make_mutable_flag<flags::subtract>(),
                        make_mutable_flag<flags::half_carry>(), make_pc_incrementer()};
        return ib.execute();
    };

    instructions[opcodes::INC_BC] = [&]() {
        Increment<BC> ib{make_mutable_register<BC>(), make_mutable_flag<flags::zero>(),
                         make_mutable_flag<flags::subtract>(),
                         make_mutable_flag<flags::half_carry>(), make_pc_incrementer()};
        return ib.execute();
    };
    instructions[opcodes::INC_DE] = [&]() {
        Increment<DE> ib{make_mutable_register<DE>(), make_mutable_flag<flags::zero>(),
                         make_mutable_flag<flags::subtract>(),
                         make_mutable_flag<flags::half_carry>(), make_pc_incrementer()};
        return ib.execute();
    };
    instructions[opcodes::INC_HL] = [&]() {
        Increment<HL> ib{make_mutable_register<HL>(), make_mutable_flag<flags::zero>(),
                         make_mutable_flag<flags::subtract>(),
                         make_mutable_flag<flags::half_carry>(), make_pc_incrementer()};
        return ib.execute();
    };
    instructions[opcodes::INC_SP] = [&]() {
        Increment<SP> ib{make_mutable_register<SP>(), make_mutable_flag<flags::zero>(),
                         make_mutable_flag<flags::subtract>(),
                         make_mutable_flag<flags::half_carry>(), make_pc_incrementer()};
        return ib.execute();
    };

    instructions[opcodes::INC_HL_INDIRECT] = [&]() {
        auto x = this->mmu.read_memory(this->registers.hl);
        // TODO this is a workaround, there should be an Increment_Byte__Indirect to avoid passing
        // a fake template type
        Increment<L> ib{MutableRegister<L>{x}, make_mutable_flag<flags::zero>(),
                        make_mutable_flag<flags::subtract>(),
                        make_mutable_flag<flags::half_carry>(), make_pc_incrementer()};
        auto elapsed_cycles = ib.execute() + 8;
        this->mmu.write_memory(this->registers.hl, x);
        return elapsed_cycles;
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
        this->xor8(this->mmu.read_memory(this->registers.hl));
        return 8;
    };
    instructions[opcodes::XOR_N] = [&]() {
        this->xor8(this->registers.a);
        return 8;
    };

    instructions[opcodes::JR_C] = [&]() {
        this->jump_r(is_flag_set(flags::carry));
        return 8;
    };
    instructions[opcodes::JR_NC] = [&]() {
        this->jump_r(!is_flag_set(flags::carry));
        return 8;
    };
    instructions[opcodes::JR_Z] = [&]() {
        this->jump_r(is_flag_set(flags::zero));
        return 8;
    };
    instructions[opcodes::JR_NZ] = [&]() {
        this->jump_r(!is_flag_set(flags::zero));
        return 8;
    };
    instructions[opcodes::LDH_C_A] = [&]() {
        this->mmu.write_memory(this->registers.c + 0xFF00, this->registers.a);
        this->registers.pc++;
        return 8;
    };
    instructions[opcodes::LDH_N_A] = [&]() {
        this->registers.pc++;
        uint16_t address = this->mmu.read_memory(this->registers.pc) + 0xFF00;
        this->registers.pc++;
        this->mmu.write_memory(address, this->registers.a);
        return 12;
    };
    instructions[opcodes::LD_HL_A] = [&]() {
        return this->save_value_to_address(this->registers.hl, this->registers.a);
    };
    instructions[opcodes::LD_HL_B] = [&]() {
        return this->save_value_to_address(this->registers.hl, this->registers.b);
    };
    instructions[opcodes::LD_HL_C] = [&]() {
        return this->save_value_to_address(this->registers.hl, this->registers.c);
    };
    instructions[opcodes::LD_HL_D] = [&]() {
        return this->save_value_to_address(this->registers.hl, this->registers.d);
    };
    instructions[opcodes::LD_HL_E] = [&]() {
        return this->save_value_to_address(this->registers.hl, this->registers.e);
    };
    instructions[opcodes::LD_HL_H] = [&]() {
        return this->save_value_to_address(this->registers.hl, this->registers.h);
    };
    instructions[opcodes::LD_HL_L] = [&]() {
        return this->save_value_to_address(this->registers.hl, this->registers.l);
    };
    instructions[opcodes::LD_A_BC] = [&]() {
        return this->save_value_to_address(this->registers.bc, this->registers.a);
    };
    instructions[opcodes::LD_A_DE] = [&]() {
        return this->save_value_to_address(this->registers.de, this->registers.a);
    };

    instructions[opcodes::CALL_NN] = [&]() {
        CallIntermediary call{Memory{mmu},
                              MutableStack{MutableMemory{mmu}, make_mutable_register<SP>()},
                              make_mutable_register<PC>()};
        return call.execute();
    };

    IncrementPC ipc{MutableRegister<PC>(registers.pc)};
    // This function is just to save typing
    auto ld_helper = [=](auto source, auto destination) {
        return [=]() {
            CopyRegister cr{source, destination, ipc};
            return cr.execute();
        };
    };
    Register<A> a{registers.a};
    Register<F> f{registers.f};
    Register<B> b{registers.b};
    Register<C> c{registers.c};
    Register<D> d{registers.d};
    Register<E> e{registers.e};
    Register<H> h{registers.h};
    Register<L> l{registers.l};
    MutableRegister<A> a_mut{registers.a};
    MutableRegister<F> f_mut{registers.f};
    MutableRegister<B> b_mut{registers.b};
    MutableRegister<C> c_mut{registers.c};
    MutableRegister<D> d_mut{registers.d};
    MutableRegister<E> e_mut{registers.e};
    MutableRegister<H> h_mut{registers.h};
    MutableRegister<L> l_mut{registers.l};
    instructions[opcodes::LD_B_B] = ld_helper(b, b_mut);
    instructions[opcodes::LD_B_C] = ld_helper(b, c_mut);
    instructions[opcodes::LD_B_D] = ld_helper(b, d_mut);
    instructions[opcodes::LD_B_E] = ld_helper(b, e_mut);
    instructions[opcodes::LD_B_H] = ld_helper(b, h_mut);
    instructions[opcodes::LD_B_L] = ld_helper(b, l_mut);
    instructions[opcodes::LD_B_A] = ld_helper(b, a_mut);
    instructions[opcodes::LD_C_B] = ld_helper(c, b_mut);
    instructions[opcodes::LD_C_C] = ld_helper(c, c_mut);
    instructions[opcodes::LD_C_D] = ld_helper(c, d_mut);
    instructions[opcodes::LD_C_E] = ld_helper(c, e_mut);
    instructions[opcodes::LD_C_H] = ld_helper(c, h_mut);
    instructions[opcodes::LD_C_L] = ld_helper(c, l_mut);
    instructions[opcodes::LD_C_A] = ld_helper(c, a_mut);
    instructions[opcodes::LD_D_B] = ld_helper(d, b_mut);
    instructions[opcodes::LD_D_C] = ld_helper(d, c_mut);
    instructions[opcodes::LD_D_D] = ld_helper(d, d_mut);
    instructions[opcodes::LD_D_E] = ld_helper(d, e_mut);
    instructions[opcodes::LD_D_H] = ld_helper(d, h_mut);
    instructions[opcodes::LD_D_L] = ld_helper(d, l_mut);
    instructions[opcodes::LD_D_A] = ld_helper(d, a_mut);
    instructions[opcodes::LD_E_B] = ld_helper(e, b_mut);
    instructions[opcodes::LD_E_C] = ld_helper(e, c_mut);
    instructions[opcodes::LD_E_D] = ld_helper(e, d_mut);
    instructions[opcodes::LD_E_E] = ld_helper(e, e_mut);
    instructions[opcodes::LD_E_H] = ld_helper(e, h_mut);
    instructions[opcodes::LD_E_L] = ld_helper(e, l_mut);
    instructions[opcodes::LD_E_A] = ld_helper(e, a_mut);
    instructions[opcodes::LD_H_B] = ld_helper(h, b_mut);
    instructions[opcodes::LD_H_C] = ld_helper(h, c_mut);
    instructions[opcodes::LD_H_D] = ld_helper(h, d_mut);
    instructions[opcodes::LD_H_E] = ld_helper(h, e_mut);
    instructions[opcodes::LD_H_H] = ld_helper(h, h_mut);
    instructions[opcodes::LD_H_L] = ld_helper(h, l_mut);
    instructions[opcodes::LD_H_A] = ld_helper(h, a_mut);
    instructions[opcodes::LD_L_B] = ld_helper(l, b_mut);
    instructions[opcodes::LD_L_C] = ld_helper(l, c_mut);
    instructions[opcodes::LD_L_D] = ld_helper(l, d_mut);
    instructions[opcodes::LD_L_E] = ld_helper(l, e_mut);
    instructions[opcodes::LD_L_H] = ld_helper(l, h_mut);
    instructions[opcodes::LD_L_L] = ld_helper(l, l_mut);
    instructions[opcodes::LD_L_A] = ld_helper(l, a_mut);
    instructions[opcodes::LD_A_B] = ld_helper(a, b_mut);
    instructions[opcodes::LD_A_C] = ld_helper(a, c_mut);
    instructions[opcodes::LD_A_D] = ld_helper(a, d_mut);
    instructions[opcodes::LD_A_E] = ld_helper(a, e_mut);
    instructions[opcodes::LD_A_H] = ld_helper(a, h_mut);
    instructions[opcodes::LD_A_L] = ld_helper(a, l_mut);
    instructions[opcodes::LD_A_A] = ld_helper(a, a_mut);
}

void Cpu::ld16(uint16_t& input) {
    registers.pc++;
    input = mmu.read_memory_word(registers.pc);
    registers.pc += 2;
}

void Cpu::ld8(uint8_t& input) {
    registers.pc++;
    const auto immediate = mmu.read_memory(registers.pc);
    input = immediate;
    registers.pc++;
}

void Cpu::ldd_hl() {
    mmu.write_memory(registers.hl, registers.a);
    registers.hl--;
    registers.pc++;
}

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

uint8_t Cpu::op_code_to_register(opcodes::OpCode opcode) {
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
        return mmu.read_memory(registers.hl);
    case 7:
        return registers.a;
    default:
        throw std::logic_error(fmt::format("Wrong register for opcode {:02x}", opcode.value));
    }
}

void Cpu::write_to_destionation(opcodes::OpCode destination, uint8_t value) {
    switch (destination.value % 8) {
    case 0:
        registers.b = value;
    case 1:
        registers.c = value;
    case 2:
        registers.d = value;
    case 3:
        registers.e = value;
    case 4:
        registers.h = value;
    case 5:
        registers.l = value;
    case 6:
        mmu.write_memory(registers.hl, value);
    case 7:
        registers.a = value;
    default:
        throw std::logic_error(fmt::format("Wrong register for opcode {:02x}", destination.value));
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
        auto val = op_code_to_register(op_code);
        bitmanip::unset(val, internal::op_code_to_bit(op_code));
        write_to_destionation(op_code, val);
        registers.pc++;
        if (was_indirect_access(op_code)) {
            return 12;
        }
        return 4;
    } else if (op_code.value >= opcodes::SET_0B.value && op_code.value <= opcodes::SET_7A.value) {
        // Set bit instruction
        auto val = op_code_to_register(op_code);
        bitmanip::set(val, internal::op_code_to_bit(op_code));
        write_to_destionation(op_code, val);
        registers.pc++;
        if (was_indirect_access(op_code)) {
            return 12;
        }
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
        auto immediate = static_cast<int8_t>(mmu.read_memory(registers.pc));
        // We need to increment here, otherwise the jump is off by one address
        registers.pc++;
        registers.pc += immediate;
    } else {
        // Set to instruction after immediate data if we didnt jump
        registers.pc++;
    }
}

int Cpu::save_value_to_address(uint16_t address, uint8_t value) {
    mmu.write_memory(registers.hl, value);
    registers.pc++;
    return 8;
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
