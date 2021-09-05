#include "cpu.hpp"
#include "bitmanipulation.hpp"
#include "instructions/callintermediary.hpp"
#include "instructions/copyregister.hpp"
#include "instructions/increment.hpp"
#include "instructions/pushregister.hpp"
#include "instructions/return.hpp"
#include "instructions/rotateleft.hpp"
#include <unordered_set>


bool Cpu::step() {
    auto opcode = opcodes::OpCode{mmu.read_byte(registers.pc)};
    auto instruction = instructions.find(opcode);
    if (instruction == instructions.end()) {
        fmt::print("Encountered unsupported opcode {:02X} at {:pc}.\n", opcode.value, registers);
        fmt::print("Ran for {} instructions.\n", instructions_executed);
        return false;
    }
    fmt::print("Executing {:02X}\n", opcode.value);
    cycles += instruction->second();
    instructions_executed++;
    return true;
}

void Cpu::set_boot_rom(const std::array<uint8_t, constants::BOOT_ROM_SIZE>& boot_rom) {
    mmu.map_boot_rom(boot_rom);
}

void Cpu::run() {
    while (step()) {
    }
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
        return this->cb(opcodes::OpCode{mmu.read_byte(registers.pc)});
    };

    instructions[opcodes::INC_A] = [&]() {
        Increment<registers::A> ib{make_mutable_register<registers::A>(),
                                   make_mutable_flag<flags::zero>(),
                                   make_mutable_flag<flags::subtract>(),
                                   make_mutable_flag<flags::half_carry>(), make_pc_incrementer()};
        return ib.execute();
    };
    instructions[opcodes::INC_B] = [&]() {
        Increment<registers::B> ib{make_mutable_register<registers::B>(),
                                   make_mutable_flag<flags::zero>(),
                                   make_mutable_flag<flags::subtract>(),
                                   make_mutable_flag<flags::half_carry>(), make_pc_incrementer()};
        return ib.execute();
    };
    instructions[opcodes::INC_C] = [&]() {
        Increment<registers::C> ib{make_mutable_register<registers::C>(),
                                   make_mutable_flag<flags::zero>(),
                                   make_mutable_flag<flags::subtract>(),
                                   make_mutable_flag<flags::half_carry>(), make_pc_incrementer()};
        return ib.execute();
    };
    instructions[opcodes::INC_D] = [&]() {
        Increment<registers::D> ib{make_mutable_register<registers::D>(),
                                   make_mutable_flag<flags::zero>(),
                                   make_mutable_flag<flags::subtract>(),
                                   make_mutable_flag<flags::half_carry>(), make_pc_incrementer()};
        return ib.execute();
    };
    instructions[opcodes::INC_E] = [&]() {
        Increment<registers::E> ib{make_mutable_register<registers::E>(),
                                   make_mutable_flag<flags::zero>(),
                                   make_mutable_flag<flags::subtract>(),
                                   make_mutable_flag<flags::half_carry>(), make_pc_incrementer()};
        return ib.execute();
    };
    instructions[opcodes::INC_H] = [&]() {
        Increment<registers::H> ib{make_mutable_register<registers::H>(),
                                   make_mutable_flag<flags::zero>(),
                                   make_mutable_flag<flags::subtract>(),
                                   make_mutable_flag<flags::half_carry>(), make_pc_incrementer()};
        return ib.execute();
    };
    instructions[opcodes::INC_L] = [&]() {
        Increment<registers::L> ib{make_mutable_register<registers::L>(),
                                   make_mutable_flag<flags::zero>(),
                                   make_mutable_flag<flags::subtract>(),
                                   make_mutable_flag<flags::half_carry>(), make_pc_incrementer()};
        return ib.execute();
    };

    instructions[opcodes::INC_BC] = [&]() {
        Increment<registers::BC> ib{make_mutable_register<registers::BC>(),
                                    make_mutable_flag<flags::zero>(),
                                    make_mutable_flag<flags::subtract>(),
                                    make_mutable_flag<flags::half_carry>(), make_pc_incrementer()};
        return ib.execute();
    };
    instructions[opcodes::INC_DE] = [&]() {
        Increment<registers::DE> ib{make_mutable_register<registers::DE>(),
                                    make_mutable_flag<flags::zero>(),
                                    make_mutable_flag<flags::subtract>(),
                                    make_mutable_flag<flags::half_carry>(), make_pc_incrementer()};
        return ib.execute();
    };
    instructions[opcodes::INC_HL] = [&]() {
        Increment<registers::HL> ib{make_mutable_register<registers::HL>(),
                                    make_mutable_flag<flags::zero>(),
                                    make_mutable_flag<flags::subtract>(),
                                    make_mutable_flag<flags::half_carry>(), make_pc_incrementer()};
        return ib.execute();
    };
    instructions[opcodes::INC_SP] = [&]() {
        Increment<registers::SP> ib{make_mutable_register<registers::SP>(),
                                    make_mutable_flag<flags::zero>(),
                                    make_mutable_flag<flags::subtract>(),
                                    make_mutable_flag<flags::half_carry>(), make_pc_incrementer()};
        return ib.execute();
    };

    instructions[opcodes::INC_HL_INDIRECT] = [&]() {
        auto x = this->mmu.read_byte(this->registers.hl);
        // TODO this is a workaround, there should be an Increment_Byte__Indirect to avoid passing
        // a fake template type
        Increment<registers::L> ib{MutableRegister<registers::L>{x},
                                   make_mutable_flag<flags::zero>(),
                                   make_mutable_flag<flags::subtract>(),
                                   make_mutable_flag<flags::half_carry>(), make_pc_incrementer()};
        auto elapsed_cycles = ib.execute() + 8;
        this->mmu.write_byte(this->registers.hl, x);
        return elapsed_cycles;
    };

    instructions[opcodes::LD_B_N] = [&]() {
        return this->ld8(registers.b);
    };
    instructions[opcodes::LD_C_N] = [&]() {
        return this->ld8(registers.c);
    };
    instructions[opcodes::LD_D_N] = [&]() {
        return this->ld8(registers.d);
    };
    instructions[opcodes::LD_E_N] = [&]() {
        return this->ld8(registers.e);
    };
    instructions[opcodes::LD_A_N] = [&]() {
        return this->ld8(registers.a);
    };
    instructions[opcodes::LD_L_N] = [&]() {
        return this->ld8(registers.l);
    };
    instructions[opcodes::LD_H_N] = [&]() {
        return this->ld8(registers.h);
    };
    instructions[opcodes::LD_BC_NN] = [&]() {
        return this->ld16(registers.bc);
    };
    instructions[opcodes::LD_DE_NN] = [&]() {
        return this->ld16(registers.de);
    };
    instructions[opcodes::LD_HL_NN] = [&]() {
        return this->ld16(registers.hl);
    };
    instructions[opcodes::LD_SP_NN] = [&]() {
        return this->ld16(registers.sp);
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
        this->xor8(this->mmu.read_byte(this->registers.hl));
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
        this->mmu.write_byte(this->registers.c + 0xFF00, this->registers.a);
        this->registers.pc++;
        return 8;
    };
    instructions[opcodes::LDH_N_A] = [&]() {
        this->registers.pc++;
        uint16_t address = this->mmu.read_byte(this->registers.pc) + 0xFF00;
        this->registers.pc++;
        this->mmu.write_byte(address, this->registers.a);
        return 12;
    };
    instructions[opcodes::LD_HL_A]
        = [&]() { return this->save_value_to_address(this->registers.hl, this->registers.a); };
    instructions[opcodes::LD_HL_B]
        = [&]() { return this->save_value_to_address(this->registers.hl, this->registers.b); };
    instructions[opcodes::LD_HL_C]
        = [&]() { return this->save_value_to_address(this->registers.hl, this->registers.c); };
    instructions[opcodes::LD_HL_D]
        = [&]() { return this->save_value_to_address(this->registers.hl, this->registers.d); };
    instructions[opcodes::LD_HL_E]
        = [&]() { return this->save_value_to_address(this->registers.hl, this->registers.e); };
    instructions[opcodes::LD_HL_H]
        = [&]() { return this->save_value_to_address(this->registers.hl, this->registers.h); };
    instructions[opcodes::LD_HL_L]
        = [&]() { return this->save_value_to_address(this->registers.hl, this->registers.l); };
    instructions[opcodes::LD_A_BC]
        = [&]() { return this->save_value_to_address(this->registers.bc, this->registers.a); };
    instructions[opcodes::LD_A_DE]
        = [&]() { return this->save_value_to_address(this->registers.de, this->registers.a); };

    instructions[opcodes::CALL_NN] = [&]() {
        CallIntermediary call{
            Memory{mmu}, MutableStack{MutableMemory{mmu}, make_mutable_register<registers::SP>()},
            make_mutable_register<registers::PC>()};
        return call.execute();
    };

    instructions[opcodes::LDI_A_HL] = [&]() { return this->indirect_hl(opcodes::LDI_A_HL); };
    instructions[opcodes::LDI_HL_A] = [&]() { return this->indirect_hl(opcodes::LDI_HL_A); };
    instructions[opcodes::LDD_A_HL] = [&]() { return this->indirect_hl(opcodes::LDD_A_HL); };
    instructions[opcodes::LDD_HL_A] = [&]() { return this->indirect_hl(opcodes::LDD_HL_A); };

    ProgramCounterIncDec ipc{MutableRegister<registers::PC>(registers.pc)};
    // This function is just to save typing
    auto ld_helper = [=](auto source, auto destination) {
        return [=]() {
            CopyRegister cr{source, destination, ipc};
            return cr.execute();
        };
    };
    Register<registers::A> a{registers.a};
    Register<registers::F> f{registers.f};
    Register<registers::B> b{registers.b};
    Register<registers::C> c{registers.c};
    Register<registers::D> d{registers.d};
    Register<registers::E> e{registers.e};
    Register<registers::H> h{registers.h};
    Register<registers::L> l{registers.l};
    Register<registers::SP> sp{registers.sp};
    Register<registers::PC> pc{registers.pc};
    Register<registers::AF> af{registers.af};
    Register<registers::BC> bc{registers.bc};
    Register<registers::DE> de{registers.de};
    Register<registers::HL> hl{registers.hl};
    MutableRegister<registers::A> a_mut{registers.a};
    MutableRegister<registers::F> f_mut{registers.f};
    MutableRegister<registers::B> b_mut{registers.b};
    MutableRegister<registers::C> c_mut{registers.c};
    MutableRegister<registers::D> d_mut{registers.d};
    MutableRegister<registers::E> e_mut{registers.e};
    MutableRegister<registers::H> h_mut{registers.h};
    MutableRegister<registers::L> l_mut{registers.l};
    MutableRegister<registers::SP> sp_mut{registers.sp};
    MutableRegister<registers::PC> pc_mut{registers.pc};
    MutableRegister<registers::AF> af_mut{registers.af};
    MutableRegister<registers::BC> bc_mut{registers.bc};
    MutableRegister<registers::DE> de_mut{registers.de};
    MutableRegister<registers::HL> hl_mut{registers.hl};
    MutableMemory mem_mut{mmu};
    MutableStack stack_mut{mem_mut, sp_mut};
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
    instructions[opcodes::PUSH_AF] = [&]() {
        PushRegisterOnStack<registers::AF> push{af, stack_mut, ProgramCounterIncDec{pc_mut}};
        return push.execute();
    };
    instructions[opcodes::PUSH_BC] = [&]() {
        PushRegisterOnStack<registers::BC> push{bc, stack_mut, ProgramCounterIncDec{pc_mut}};
        return push.execute();
    };
    instructions[opcodes::PUSH_DE] = [&]() {
        PushRegisterOnStack<registers::DE> push{de, stack_mut, ProgramCounterIncDec{pc_mut}};
        return push.execute();
    };
    instructions[opcodes::PUSH_HL] = [&]() {
        PushRegisterOnStack<registers::HL> push{hl, stack_mut, ProgramCounterIncDec{pc_mut}};
        return push.execute();
    };

    instructions[opcodes::RET] = [&]() {
        Return r(stack_mut, ProgramCounterJump{pc_mut});
        return r.execute();
    };
}

unsigned int Cpu::ld16(uint16_t& input) {
    registers.pc++;
    input = mmu.read_word(registers.pc);
    registers.pc += 2;
    return 12;
}

unsigned int Cpu::ld8(uint8_t& input) {
    registers.pc++;
    const auto immediate = mmu.read_byte(registers.pc);
    input = immediate;
    registers.pc++;
    return 8;
}

int Cpu::indirect_hl(opcodes::OpCode op) {
    if (op == opcodes::LDD_HL_A or op == opcodes::LDI_HL_A) {
        mmu.write_byte(registers.hl, registers.a);
    } else if (op == opcodes::LDD_A_HL or op == opcodes::LDI_A_HL) {
        registers.a = mmu.read_byte(registers.hl);
    }
    if (op == opcodes::LDD_HL_A or op == opcodes::LDD_A_HL) {
        registers.hl--;
    } else {
        registers.hl++;
    }
    registers.pc++;
    return 8;
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
        return mmu.read_byte(registers.hl);
    case 7:
        return registers.a;
    default:
        throw std::logic_error(fmt::format("Wrong register for opcode {:02x}", opcode.value));
    }
}

void Cpu::write_to_destination(opcodes::OpCode destination, uint8_t value) {
    switch (destination.value % 8) {
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
        mmu.write_byte(registers.hl, value);
        break;
    case 7:
        registers.a = value;
        break;
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
        write_to_destination(op_code, val);
        registers.pc++;
        if (was_indirect_access(op_code)) {
            return 12;
        }
        return 4;
    } else if (op_code.value >= opcodes::SET_0B.value && op_code.value <= opcodes::SET_7A.value) {
        // Set bit instruction
        auto val = op_code_to_register(op_code);
        bitmanip::set(val, internal::op_code_to_bit(op_code));
        write_to_destination(op_code, val);
        registers.pc++;
        if (was_indirect_access(op_code)) {
            return 12;
        }
        return 4;
    } else if (std::unordered_set{opcodes::RL_A, opcodes::RL_B, opcodes::RL_C, opcodes::RL_D,
                                  opcodes::RL_E, opcodes::RL_H, opcodes::RL_L}
                   .contains(op_code)) {
        auto z_flag = make_mutable_flag<flags::zero>();
        auto s_flag = make_mutable_flag<flags::subtract>();
        auto hc_flag = make_mutable_flag<flags::half_carry>();
        auto c_flag = make_mutable_flag<flags::carry>();
        if (op_code == opcodes::RL_A) {
            RotateLeft<registers::A> rla(z_flag, s_flag, hc_flag, c_flag,
                                         make_mutable_register<registers::A>());
            return rla.execute();
        }
        if (op_code == opcodes::RL_B) {
            RotateLeft<registers::B> rlb(z_flag, s_flag, hc_flag, c_flag,
                                         make_mutable_register<registers::B>());
            return rlb.execute();
        }
        if (op_code == opcodes::RL_C) {
            RotateLeft<registers::C> rlc(z_flag, s_flag, hc_flag, c_flag,
                                         make_mutable_register<registers::C>());
            return rlc.execute();
        }
        if (op_code == opcodes::RL_D) {
            RotateLeft<registers::D> rld(z_flag, s_flag, hc_flag, c_flag,
                                         make_mutable_register<registers::D>());
            return rld.execute();
        }
        if (op_code == opcodes::RL_E) {
            RotateLeft<registers::E> rle(z_flag, s_flag, hc_flag, c_flag,
                                         make_mutable_register<registers::E>());
            return rle.execute();
        }
        if (op_code == opcodes::RL_H) {
            RotateLeft<registers::H> rlh(z_flag, s_flag, hc_flag, c_flag,
                                         make_mutable_register<registers::H>());
            return rlh.execute();
        }
        if (op_code == opcodes::RL_L) {
            RotateLeft<registers::L> rll(z_flag, s_flag, hc_flag, c_flag,
                                         make_mutable_register<registers::L>());
            return rll.execute();
        }
    } else {
        // TODO implement further CB instructions
        throw std::runtime_error(fmt::format("CB {:02x} not supported\n", op_code.value));
    }
    return 0;
}

void Cpu::jump_r(bool condition_met) {
    registers.pc++;
    if (condition_met) {
        // For C++20 this is defined behaviour since signed integers are twos complement
        // On older standards this is implementation defined
        // https://stackoverflow.com/questions/13150449/efficient-unsigned-to-signed-cast-avoiding-implementation-defined-behavior
        auto immediate = static_cast<int8_t>(mmu.read_byte(registers.pc));
        // We need to increment here, otherwise the jump is off by one address
        registers.pc++;
        registers.pc += immediate;
    } else {
        // Set to instruction after immediate data if we didnt jump
        registers.pc++;
    }
}

int Cpu::save_value_to_address(uint16_t address, uint8_t value) {
    mmu.write_byte(address, value);
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
    return ((opcode.value & 0xF0) >> constants::NIBBLE_SIZE) * 2
           + ((opcode.value & 0x0F) / constants::BYTE_SIZE);
}
