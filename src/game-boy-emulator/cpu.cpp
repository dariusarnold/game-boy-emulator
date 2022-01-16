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
    auto opcode = fetch();
    auto instruction = decode(opcode);
    if (not instruction) {
        print(fmt::format("Encountered unsupported opcode {:02X} at {:pc}.\n", opcode.value, registers), Verbosity::LEVEL_ERROR);
        print(get_minimal_debug_state() + "\n", Verbosity::LEVEL_ERROR);
        print(fmt::format("Ran for {} instructions.\n", instructions_executed), Verbosity::LEVEL_INFO);
        return false;
    }
    print(fmt::format("Executing {} 0x{:02X}\n", opcode.extendend ? "0xCB" : "", opcode.value), Verbosity::LEVEL_INFO);
    registers.pc += opcode.extendend ? 2 : 1;
    cycles += instruction();
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
}

Cpu::Cpu(): Cpu(Verbosity::LEVEL_INFO){}

Cpu::Cpu(Verbosity verbosity_): verbosity(verbosity_) {
    instructions[opcodes::NOP] = [&]() {
        return 4;
    };

    instructions[opcodes::INC_A] = [&]() {
        Increment<registers::A> ib{make_mutable_register<registers::A>(),
                                   make_mutable_flag<flags::zero>(),
                                   make_mutable_flag<flags::subtract>(),
                                   make_mutable_flag<flags::half_carry>()};
        return ib.execute();
    };
    instructions[opcodes::INC_B] = [&]() {
        Increment<registers::B> ib{make_mutable_register<registers::B>(),
                                   make_mutable_flag<flags::zero>(),
                                   make_mutable_flag<flags::subtract>(),
                                   make_mutable_flag<flags::half_carry>()};
        return ib.execute();
    };
    instructions[opcodes::INC_C] = [&]() {
        Increment<registers::C> ib{make_mutable_register<registers::C>(),
                                   make_mutable_flag<flags::zero>(),
                                   make_mutable_flag<flags::subtract>(),
                                   make_mutable_flag<flags::half_carry>()};
        return ib.execute();
    };
    instructions[opcodes::INC_D] = [&]() {
        Increment<registers::D> ib{make_mutable_register<registers::D>(),
                                   make_mutable_flag<flags::zero>(),
                                   make_mutable_flag<flags::subtract>(),
                                   make_mutable_flag<flags::half_carry>()};
        return ib.execute();
    };
    instructions[opcodes::INC_E] = [&]() {
        Increment<registers::E> ib{make_mutable_register<registers::E>(),
                                   make_mutable_flag<flags::zero>(),
                                   make_mutable_flag<flags::subtract>(),
                                   make_mutable_flag<flags::half_carry>()};
        return ib.execute();
    };
    instructions[opcodes::INC_H] = [&]() {
        Increment<registers::H> ib{make_mutable_register<registers::H>(),
                                   make_mutable_flag<flags::zero>(),
                                   make_mutable_flag<flags::subtract>(),
                                   make_mutable_flag<flags::half_carry>()};
        return ib.execute();
    };
    instructions[opcodes::INC_L] = [&]() {
        Increment<registers::L> ib{make_mutable_register<registers::L>(),
                                   make_mutable_flag<flags::zero>(),
                                   make_mutable_flag<flags::subtract>(),
                                   make_mutable_flag<flags::half_carry>()};
        return ib.execute();
    };

    instructions[opcodes::INC_BC] = [&]() {
        Increment<registers::BC> ib{make_mutable_register<registers::BC>(),
                                    make_mutable_flag<flags::zero>(),
                                    make_mutable_flag<flags::subtract>(),
                                    make_mutable_flag<flags::half_carry>()};
        return ib.execute();
    };
    instructions[opcodes::INC_DE] = [&]() {
        Increment<registers::DE> ib{make_mutable_register<registers::DE>(),
                                    make_mutable_flag<flags::zero>(),
                                    make_mutable_flag<flags::subtract>(),
                                    make_mutable_flag<flags::half_carry>()};
        return ib.execute();
    };
    instructions[opcodes::INC_HL] = [&]() {
        Increment<registers::HL> ib{make_mutable_register<registers::HL>(),
                                    make_mutable_flag<flags::zero>(),
                                    make_mutable_flag<flags::subtract>(),
                                    make_mutable_flag<flags::half_carry>()};
        return ib.execute();
    };
    instructions[opcodes::INC_SP] = [&]() {
        Increment<registers::SP> ib{make_mutable_register<registers::SP>(),
                                    make_mutable_flag<flags::zero>(),
                                    make_mutable_flag<flags::subtract>(),
                                    make_mutable_flag<flags::half_carry>()};
        return ib.execute();
    };

    instructions[opcodes::INC_HL_INDIRECT] = [&]() {
        auto x = this->mmu.read_byte(this->registers.hl);
        // TODO this is a workaround, there should be an Increment_Byte__Indirect to avoid passing
        // a fake template type
        Increment<registers::L> ib{MutableRegister<registers::L>{x},
                                   make_mutable_flag<flags::zero>(),
                                   make_mutable_flag<flags::subtract>(),
                                   make_mutable_flag<flags::half_carry>()};
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
        return this->jump_r(is_flag_set(flags::carry));
    };
    instructions[opcodes::JR_NC] = [&]() {
        return this->jump_r(!is_flag_set(flags::carry));
    };
    instructions[opcodes::JR_Z] = [&]() {
        return this->jump_r(is_flag_set(flags::zero));
    };
    instructions[opcodes::JR_NZ] = [&]() {
        return this->jump_r(!is_flag_set(flags::zero));
    };
    instructions[opcodes::JR] = [&]() { return this->jump_r(true); };
    instructions[opcodes::LDH_C_A] = [&]() {
        this->mmu.write_byte(this->registers.c + 0xFF00, this->registers.a);
        return 8;
    };
    instructions[opcodes::LDH_N_A] = [&]() {
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
        = [&]() { return this->load_value_from_address(this->registers.bc, this->registers.a); };
    instructions[opcodes::LD_A_DE]
        = [&]() { return this->load_value_from_address(this->registers.de, this->registers.a); };

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

    // This function is just to save typing
    auto ld_helper = [=](auto source, auto destination) {
        return [=]() {
            CopyRegister cr{source, destination};
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
    instructions[opcodes::PUSH_AF] = [=]() {
        PushRegisterOnStack<registers::AF> push{af, stack_mut};
        return push.execute();
    };
    instructions[opcodes::PUSH_BC] = [=]() {
        PushRegisterOnStack<registers::BC> push{bc, stack_mut};
        return push.execute();
    };
    instructions[opcodes::PUSH_DE] = [=]() {
        PushRegisterOnStack<registers::DE> push{de, stack_mut};
        return push.execute();
    };
    instructions[opcodes::PUSH_HL] = [=]() {
        PushRegisterOnStack<registers::HL> push{hl, stack_mut};
        return push.execute();
    };

    instructions[opcodes::RET] = [=]() {
        Return r(stack_mut, ProgramCounterJump{pc_mut});
        return r.execute();
    };
}

t_cycle Cpu::ld16(uint16_t& input) {
    input = mmu.read_word(registers.pc);
    registers.pc += 2;
    return 12;
}

t_cycle Cpu::ld8(uint8_t& input) {
    const auto immediate = mmu.read_byte(registers.pc);
    input = immediate;
    registers.pc++;
    return 8;
}

t_cycle Cpu::indirect_hl(opcodes::OpCode op) {
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
        return mmu.read_byte(registers.hl);
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
        mmu.write_byte(registers.hl, value);
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

t_cycle Cpu::cb(uint8_t instruction_byte) {
    if (instruction_byte >= opcodes::BIT_0B.value && instruction_byte <= opcodes::BIT_7A.value) {
        // Set bit instruction
        test_bit(op_code_to_register(instruction_byte), internal::op_code_to_bit(instruction_byte));
        if (was_indirect_access(instruction_byte)) {
            return 12;
        }
        return 4;
    } else if (instruction_byte >= opcodes::RES_0B.value && instruction_byte <= opcodes::RES_7A.value) {
        // Reset bit instruction
        auto val = op_code_to_register(instruction_byte);
        bitmanip::unset(val, internal::op_code_to_bit(instruction_byte));
        write_to_destination(instruction_byte, val);
        if (was_indirect_access(instruction_byte)) {
            return 12;
        }
        return 4;
    } else if (instruction_byte >= opcodes::SET_0B.value && instruction_byte <= opcodes::SET_7A.value) {
        // Set bit instruction
        auto val = op_code_to_register(instruction_byte);
        bitmanip::set(val, internal::op_code_to_bit(instruction_byte));
        write_to_destination(instruction_byte, val);
        if (was_indirect_access(instruction_byte)) {
            return 12;
        }
        return 4;
    } else if (opcodes::RL_A.value == instruction_byte or opcodes::RL_B.value == instruction_byte
               or opcodes::RL_C.value == instruction_byte or opcodes::RL_D.value == instruction_byte
               or opcodes::RL_E.value == instruction_byte or opcodes::RL_H.value == instruction_byte
               or opcodes::RL_L.value == instruction_byte) {
        auto z_flag = make_mutable_flag<flags::zero>();
        auto s_flag = make_mutable_flag<flags::subtract>();
        auto hc_flag = make_mutable_flag<flags::half_carry>();
        auto c_flag = make_mutable_flag<flags::carry>();
        if (instruction_byte == opcodes::RL_A.value) {
            RotateLeft<registers::A> rla(z_flag, s_flag, hc_flag, c_flag,
                                         make_mutable_register<registers::A>());
            return rla.execute();
        }
        if (instruction_byte == opcodes::RL_B.value) {
            RotateLeft<registers::B> rlb(z_flag, s_flag, hc_flag, c_flag,
                                         make_mutable_register<registers::B>());
            return rlb.execute();
        }
        if (instruction_byte == opcodes::RL_C.value) {
            RotateLeft<registers::C> rlc(z_flag, s_flag, hc_flag, c_flag,
                                         make_mutable_register<registers::C>());
            return rlc.execute();
        }
        if (instruction_byte == opcodes::RL_D.value) {
            RotateLeft<registers::D> rld(z_flag, s_flag, hc_flag, c_flag,
                                         make_mutable_register<registers::D>());
            return rld.execute();
        }
        if (instruction_byte == opcodes::RL_E.value) {
            RotateLeft<registers::E> rle(z_flag, s_flag, hc_flag, c_flag,
                                         make_mutable_register<registers::E>());
            return rle.execute();
        }
        if (instruction_byte == opcodes::RL_H.value) {
            RotateLeft<registers::H> rlh(z_flag, s_flag, hc_flag, c_flag,
                                         make_mutable_register<registers::H>());
            return rlh.execute();
        }
        if (instruction_byte == opcodes::RL_L.value) {
            RotateLeft<registers::L> rll(z_flag, s_flag, hc_flag, c_flag,
                                         make_mutable_register<registers::L>());
            return rll.execute();
        }
    } else {
        // TODO implement further CB instructions
        throw std::runtime_error(fmt::format("CB {:02x} not supported\n", instruction_byte));
    }
    return 0;
}

t_cycle Cpu::jump_r(bool condition_met) {
    // For C++20 this is defined behaviour since signed integers are twos complement
    // On older standards this is implementation defined
    // https://stackoverflow.com/questions/13150449/efficient-unsigned-to-signed-cast-avoiding-implementation-defined-behavior
    auto immediate = static_cast<int8_t>(mmu.read_byte(registers.pc));
    // We need to increment here, otherwise the jump is off by one address.
    // Reading = incrementing happens whether the jump happens or not.
    registers.pc++;
    if (condition_met) {
        registers.pc += immediate;
    }
    // If the action was taken, one additional m-cycle is used.
    if (condition_met) {
        return 12;
    }
    return 8;
}

t_cycle Cpu::save_value_to_address(uint16_t address, uint8_t value) {
    mmu.write_byte(address, value);
    return 8;
}

t_cycle Cpu::load_value_from_address(uint16_t address, uint8_t& value) {
    value = mmu.read_byte(address);
    return 8;
}

opcodes::OpCode Cpu::fetch() {
    auto opcode = opcodes::OpCode{mmu.read_byte(registers.pc)};
    if (opcode == opcodes::CB) {
        opcode = opcodes::OpCode{mmu.read_byte(registers.pc + 1), true};
    }
    return opcode;
}

Cpu::Instruction Cpu::decode(opcodes::OpCode opcode) {
    if (opcode.extendend) {
        return [&, opcode]() {
            auto v = opcode.value;
            return this->cb(v);
        };
    }
    auto instruction = instructions.find(opcode);
    if (instruction == instructions.end()) {
        return {};
    }
    return instruction->second;
}

std::string Cpu::get_minimal_debug_state() {
    // Format: [registers] (mem[pc] mem[pc+1] mem[pc+2] mem[pc+3])
    // Thanks to https://github.com/wheremyfoodat/Gameboy-logs
    return fmt::format("A: {:02X} F: {:02X} B: {:02X} C: {:02X} D: {:02X} E: {:02X} H: {:02X} L: "
                       "{:02X} SP: {:04X} "
                       "PC: 00:{:04X} ({:02X} {:02X} {:02X} {:02X})",
                       registers.a, registers.f, registers.b, registers.c, registers.d, registers.e,
                       registers.h, registers.l, registers.sp, registers.pc,
                       mmu.read_byte(registers.pc), mmu.read_byte(registers.pc + 1),
                       mmu.read_byte(registers.pc + 2), mmu.read_byte(registers.pc + 3));
}

void Cpu::print(std::string_view message, Verbosity level) {
    if (static_cast<int>(level) <= static_cast<int>(verbosity)) {
        fmt::print(message);
    }
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
