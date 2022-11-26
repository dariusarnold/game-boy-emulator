#include "cpu.hpp"
#include "addressbus.hpp"
#include "bitmanipulation.hpp"
#include "emulator.hpp"
#include "exceptions.hpp"
#include "instructions/callimmediate.hpp"
#include "instructions/copyregister.hpp"
#include "instructions/increment.hpp"
#include "instructions/loadimmediateword.hpp"
#include "instructions/popstack.hpp"
#include "instructions/pushregister.hpp"
#include "instructions/return.hpp"
#include "instructions/rotateleft.hpp"

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

void Cpu::xor8(uint8_t value) {
    registers.a ^= value;
    if (registers.a == 0) {
        set_zero_flag(BitValues::Active);
    }
    set_subtract_flag(BitValues::Inactive);
    set_half_carry_flag(BitValues::Inactive);
    set_carry_flag(BitValues::Inactive);
}

Cpu::Cpu(Emulator* emulator): Cpu(emulator,Verbosity::LEVEL_INFO){}

Cpu::Cpu(Emulator* emulator, Verbosity verbosity_) : m_emulator(emulator), verbosity(verbosity_) {
    /*
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
    Register<registers::A> a_mut{registers.a};
    Register<registers::F> f_mut{registers.f};
    Register<registers::B> b_mut{registers.b};
    Register<registers::C> c_mut{registers.c};
    Register<registers::D> d_mut{registers.d};
    Register<registers::E> e_mut{registers.e};
    Register<registers::H> h_mut{registers.h};
    Register<registers::L> l_mut{registers.l};
    Register<registers::SP> sp_mut{registers.sp};
    Register<registers::PC> pc_mut{registers.pc};
    Register<registers::AF> af_mut{registers.af};
    Register<registers::BC> bc_mut{registers.bc};
    Register<registers::DE> de_mut{registers.de};
    Register<registers::HL> hl_mut{registers.hl};
    MutableMemory mem_mut{*m_emulator->get_mmu().get()};
    Memory mem{*m_emulator->get_mmu().get()};
    MutableStack stack_mut{mem_mut, sp_mut};

    instructions[opcodes::NOP] = [&]() {
        return 4;
    };

    instructions[opcodes::INC_A] = [&]() {
        IncrementByte<registers::A> ib{make_mutable_register<registers::A>(),
                                   make_mutable_flag<flags::zero>(),
                                   make_mutable_flag<flags::subtract>(),
                                   make_mutable_flag<flags::half_carry>()};
        return ib.execute();
    };
    instructions[opcodes::INC_B] = [&]() {
        IncrementByte<registers::B> ib{make_mutable_register<registers::B>(),
                                   make_mutable_flag<flags::zero>(),
                                   make_mutable_flag<flags::subtract>(),
                                   make_mutable_flag<flags::half_carry>()};
        return ib.execute();
    };
    instructions[opcodes::INC_C] = [&]() {
        IncrementByte<registers::C> ib{make_mutable_register<registers::C>(),
                                   make_mutable_flag<flags::zero>(),
                                   make_mutable_flag<flags::subtract>(),
                                   make_mutable_flag<flags::half_carry>()};
        return ib.execute();
    };
    instructions[opcodes::INC_D] = [&]() {
        IncrementByte<registers::D> ib{make_mutable_register<registers::D>(),
                                   make_mutable_flag<flags::zero>(),
                                   make_mutable_flag<flags::subtract>(),
                                   make_mutable_flag<flags::half_carry>()};
        return ib.execute();
    };
    instructions[opcodes::INC_E] = [&]() {
        IncrementByte<registers::E> ib{make_mutable_register<registers::E>(),
                                   make_mutable_flag<flags::zero>(),
                                   make_mutable_flag<flags::subtract>(),
                                   make_mutable_flag<flags::half_carry>()};
        return ib.execute();
    };
    instructions[opcodes::INC_H] = [&]() {
        IncrementByte<registers::H> ib{make_mutable_register<registers::H>(),
                                   make_mutable_flag<flags::zero>(),
                                   make_mutable_flag<flags::subtract>(),
                                   make_mutable_flag<flags::half_carry>()};
        return ib.execute();
    };
    instructions[opcodes::INC_L] = [&]() {
        IncrementByte<registers::L> ib{make_mutable_register<registers::L>(),
                                   make_mutable_flag<flags::zero>(),
                                   make_mutable_flag<flags::subtract>(),
                                   make_mutable_flag<flags::half_carry>()};
        return ib.execute();
    };

    instructions[opcodes::INC_BC] = [&]() {
        IncrementWord<registers::BC> ib{make_mutable_register<registers::BC>()};
        return ib.execute();
    };
    instructions[opcodes::INC_DE] = [&]() {
        IncrementWord<registers::DE> ib{make_mutable_register<registers::DE>()};
        return ib.execute();
    };
    instructions[opcodes::INC_HL] = [&]() {
        IncrementWord<registers::HL> ib{make_mutable_register<registers::HL>()};
        return ib.execute();
    };
    instructions[opcodes::INC_SP] = [&]() {
        IncrementWord<registers::SP> ib{make_mutable_register<registers::SP>()};
        return ib.execute();
    };

    instructions[opcodes::INC_HL_INDIRECT] = [&]() {
        auto x = this->m_mmu.read_byte(this->registers.hl);
        // TODO this is a workaround, there should be an Increment_Byte__Indirect to avoid passing
        // a fake template type
        IncrementByte<registers::L> ib{Register<registers::L>{x},
                                   make_mutable_flag<flags::zero>(),
                                   make_mutable_flag<flags::subtract>(),
                                   make_mutable_flag<flags::half_carry>()};
        auto elapsed_cycles = ib.execute() + 8;
        this->m_mmu.write_byte(this->registers.hl, x);
        return elapsed_cycles;
    };

    instructions[opcodes::DEC_A] = [this]() {
        this->dec8(registers.a);
        return 4;
    };

    instructions[opcodes::DEC_B] = [this]() {
        this->dec8(registers.b);
        return 4;
    };

    instructions[opcodes::DEC_C] = [this]() {
        this->dec8(registers.c);
        return 4;
    };

    instructions[opcodes::DEC_D] = [this]() {
        this->dec8(registers.d);
        return 4;
    };

    instructions[opcodes::DEC_E] = [this]() {
        this->dec8(registers.e);
        return 4;
    };

    instructions[opcodes::DEC_H] = [this]() {
        this->dec8(registers.h);
        return 4;
    };

    instructions[opcodes::DEC_L] = [this]() {
        this->dec8(registers.l);
        return 4;
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
    instructions[opcodes::LD_BC_NN] = [=]() {
        LoadImmediateWord load_immediate_word{bc_mut, pc_mut, stack_mut, mem};
        return load_immediate_word.execute();
    };
    instructions[opcodes::LD_DE_NN] = [=]() {
        LoadImmediateWord load_immediate_word{de_mut, pc_mut, stack_mut, mem};
        return load_immediate_word.execute();
    };
    instructions[opcodes::LD_HL_NN] = [=]() {
        LoadImmediateWord load_immediate_word{hl_mut, pc_mut, stack_mut, mem};
        return load_immediate_word.execute();
    };
    instructions[opcodes::LD_SP_NN] = [=]() {
        LoadImmediateWord load_immediate_word{sp_mut, pc_mut, stack_mut, mem};
        return load_immediate_word.execute();
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
        this->xor8(this->m_mmu.read_byte(this->registers.hl));
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
        this->m_mmu.write_byte(this->registers.c + 0xFF00, this->registers.a);
        return 8;
    };
    instructions[opcodes::LDH_N_A] = [&]() {
        uint16_t address = this->m_mmu.read_byte(this->registers.pc) + 0xFF00;
        this->registers.pc++;
        this->m_mmu.write_byte(address, this->registers.a);
        return 12;
    };
    instructions[opcodes::LDH_A_N] = [this]() {
        const uint16_t address = m_mmu.read_byte(registers.pc) + 0xFF00;
        registers.pc++;
        registers.a = m_mmu.read_byte(address);
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
        CallImmediate call{
            Memory{m_mmu}, MutableStack{MutableMemory{m_mmu}, make_mutable_register<registers::SP>()},
            make_mutable_register<registers::PC>()};
        return call.execute();
    };

    instructions[opcodes::LDI_A_HL] = [&]() { return this->indirect_hl(opcodes::LDI_A_HL); };
    instructions[opcodes::LDI_HL_A] = [&]() { return this->indirect_hl(opcodes::LDI_HL_A); };
    instructions[opcodes::LDD_A_HL] = [&]() { return this->indirect_hl(opcodes::LDD_A_HL); };
    instructions[opcodes::LDD_HL_A] = [&]() { return this->indirect_hl(opcodes::LDD_HL_A); };

    instructions[opcodes::LD_B_B] = ld_helper(b, b_mut);
    instructions[opcodes::LD_B_C] = ld_helper(c, b_mut);
    instructions[opcodes::LD_B_D] = ld_helper(d, b_mut);
    instructions[opcodes::LD_B_E] = ld_helper(e, b_mut);
    instructions[opcodes::LD_B_H] = ld_helper(h, b_mut);
    instructions[opcodes::LD_B_L] = ld_helper(l, b_mut);
    instructions[opcodes::LD_B_A] = ld_helper(a, b_mut);
    instructions[opcodes::LD_C_B] = ld_helper(b, c_mut);
    instructions[opcodes::LD_C_C] = ld_helper(c, c_mut);
    instructions[opcodes::LD_C_D] = ld_helper(d, c_mut);
    instructions[opcodes::LD_C_E] = ld_helper(e, c_mut);
    instructions[opcodes::LD_C_H] = ld_helper(h, c_mut);
    instructions[opcodes::LD_C_L] = ld_helper(l, c_mut);
    instructions[opcodes::LD_C_A] = ld_helper(a, c_mut);
    instructions[opcodes::LD_D_B] = ld_helper(b, d_mut);
    instructions[opcodes::LD_D_C] = ld_helper(c, d_mut);
    instructions[opcodes::LD_D_D] = ld_helper(d, d_mut);
    instructions[opcodes::LD_D_E] = ld_helper(e, d_mut);
    instructions[opcodes::LD_D_H] = ld_helper(h, d_mut);
    instructions[opcodes::LD_D_L] = ld_helper(l, d_mut);
    instructions[opcodes::LD_D_A] = ld_helper(a, d_mut);
    instructions[opcodes::LD_E_B] = ld_helper(b, e_mut);
    instructions[opcodes::LD_E_C] = ld_helper(c, e_mut);
    instructions[opcodes::LD_E_D] = ld_helper(d, e_mut);
    instructions[opcodes::LD_E_E] = ld_helper(e, e_mut);
    instructions[opcodes::LD_E_H] = ld_helper(h, e_mut);
    instructions[opcodes::LD_E_L] = ld_helper(l, e_mut);
    instructions[opcodes::LD_E_A] = ld_helper(a, e_mut);
    instructions[opcodes::LD_H_B] = ld_helper(b, h_mut);
    instructions[opcodes::LD_H_C] = ld_helper(c, h_mut);
    instructions[opcodes::LD_H_D] = ld_helper(d, h_mut);
    instructions[opcodes::LD_H_E] = ld_helper(e, h_mut);
    instructions[opcodes::LD_H_H] = ld_helper(h, h_mut);
    instructions[opcodes::LD_H_L] = ld_helper(l, h_mut);
    instructions[opcodes::LD_H_A] = ld_helper(a, h_mut);
    instructions[opcodes::LD_L_B] = ld_helper(b, l_mut);
    instructions[opcodes::LD_L_C] = ld_helper(c, l_mut);
    instructions[opcodes::LD_L_D] = ld_helper(d, l_mut);
    instructions[opcodes::LD_L_E] = ld_helper(e, l_mut);
    instructions[opcodes::LD_L_H] = ld_helper(h, l_mut);
    instructions[opcodes::LD_L_L] = ld_helper(l, l_mut);
    instructions[opcodes::LD_L_A] = ld_helper(a, l_mut);
    instructions[opcodes::LD_A_B] = ld_helper(b, a_mut);
    instructions[opcodes::LD_A_C] = ld_helper(c, a_mut);
    instructions[opcodes::LD_A_D] = ld_helper(d, a_mut);
    instructions[opcodes::LD_A_E] = ld_helper(e, a_mut);
    instructions[opcodes::LD_A_H] = ld_helper(h, a_mut);
    instructions[opcodes::LD_A_L] = ld_helper(l, a_mut);
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

    instructions[opcodes::RL_A] = [&]() {
        auto z_flag = make_mutable_flag<flags::zero>();
        auto s_flag = make_mutable_flag<flags::subtract>();
        auto hc_flag = make_mutable_flag<flags::half_carry>();
        auto c_flag = make_mutable_flag<flags::carry>();
        RotateLeft<registers::A> rla(z_flag, s_flag, hc_flag, c_flag,
                                     make_mutable_register<registers::A>(), true,
                                     ZeroFlagSetMode::AlwaysOff);
        return rla.execute();
    };

    instructions[opcodes::POP_BC] = [=]() {
        PopStack<registers::BC> pop_stack{bc_mut, stack_mut};
        return pop_stack.execute();
    };
    instructions[opcodes::POP_DE] = [=]() {
        PopStack<registers::DE> pop_stack{de_mut, stack_mut};
        return pop_stack.execute();
    };
    instructions[opcodes::POP_HL] = [=]() {
        PopStack<registers::HL> pop_stack{hl_mut, stack_mut};
        return pop_stack.execute();
    };

    instructions[opcodes::CP_A] = [this](){ return cp(registers.a, registers.a); };
    instructions[opcodes::CP_B] = [this](){ return cp(registers.a, registers.b); };
    instructions[opcodes::CP_C] = [this](){ return cp(registers.a, registers.c); };
    instructions[opcodes::CP_D] = [this](){ return cp(registers.a, registers.d); };
    instructions[opcodes::CP_E] = [this](){ return cp(registers.a, registers.e); };
    instructions[opcodes::CP_H] = [this](){ return cp(registers.a, registers.h); };
    instructions[opcodes::CP_L] = [this](){ return cp(registers.a, registers.l); };
    instructions[opcodes::CP_HL] = [this]() {
        auto value = m_mmu.read_byte(registers.hl);
        return cp(registers.a, value);
    };
    instructions[opcodes::CP_N] = [&](){
        auto value = m_mmu.read_byte(registers.pc);
        registers.pc++;
        return cp(registers.a, value);
    };
    instructions[opcodes::LD_NN_A] = [this]() {
        const auto address = m_mmu.read_word(registers.pc);
        registers.pc += 2;
        m_mmu.write_byte(address, registers.a);
        return 16;
    };
    instructions[opcodes::LD_A_NN] = [this]() {
        const auto address = m_mmu.read_word(registers.pc);
        registers.pc += 2;
        registers.a = m_mmu.read_byte(address);
        return 16;
    };
    instructions[opcodes::SUB_A_A] = [this]() { return sub(registers.a); };
    instructions[opcodes::SUB_A_B] = [this]() { return sub(registers.b); };
    instructions[opcodes::SUB_A_C] = [this]() { return sub(registers.c); };
    instructions[opcodes::SUB_A_D] = [this]() { return sub(registers.d); };
    instructions[opcodes::SUB_A_E] = [this]() { return sub(registers.e); };
    instructions[opcodes::SUB_A_H] = [this]() { return sub(registers.h); };
    instructions[opcodes::SUB_A_L] = [this]() { return sub(registers.l); };
    instructions[opcodes::SUB_A_HL] = [this]() {
        auto value = m_mmu.read_byte(registers.hl);
        return sub(value);
    };
    instructions[opcodes::ADD_A_A] = [this]() { return add(registers.a); };
    instructions[opcodes::ADD_A_B] = [this]() { return add(registers.b); };
    instructions[opcodes::ADD_A_C] = [this]() { return add(registers.c); };
    instructions[opcodes::ADD_A_D] = [this]() { return add(registers.d); };
    instructions[opcodes::ADD_A_E] = [this]() { return add(registers.e); };
    instructions[opcodes::ADD_A_H] = [this]() { return add(registers.h); };
    instructions[opcodes::ADD_A_L] = [this]() { return add(registers.l); };
    instructions[opcodes::ADD_A_HL] = [this]() {
        auto value = m_mmu.read_byte(registers.hl);
        return add(value);
    };
    instructions[opcodes::JP_NN] = [this]() {
        auto address = m_mmu.read_word(registers.pc);
        registers.pc = address;
        return 16;
    };
    instructions[opcodes::DI] = [this]() {
        interrupt_handler.set_enable_flag(false);
        return 4;
    };
     */
}

//t_cycle Cpu::ld8(uint8_t& input) {
//    const auto immediate = m_mmu.read_byte(registers.pc);
//    input = immediate;
//    registers.pc++;
//    return 8;
//}

//t_cycle Cpu::indirect_hl(opcodes::OpCode op) {
//    if (op == opcodes::LDD_HL_A or op == opcodes::LDI_HL_A) {
//        m_mmu.write_byte(registers.hl, registers.a);
//    } else if (op == opcodes::LDD_A_HL or op == opcodes::LDI_A_HL) {
//        registers.a = m_mmu.read_byte(registers.hl);
//    }
//    if (op == opcodes::LDD_HL_A or op == opcodes::LDD_A_HL) {
//        registers.hl--;
//    } else {
//        registers.hl++;
//    }
//    return 8;
//}

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

//t_cycle Cpu::jump_r(bool condition_met) {
//    // For C++20 this is defined behaviour since signed integers are twos complement
//    // On older standards this is implementation defined
//    // https://stackoverflow.com/questions/13150449/efficient-unsigned-to-signed-cast-avoiding-implementation-defined-behavior
//    auto immediate = static_cast<int8_t>(m_mmu.read_byte(registers.pc));
//    // We need to increment here, otherwise the jump is off by one address.
//    // Reading = incrementing happens whether the jump happens or not.
//    registers.pc++;
//    if (condition_met) {
//        registers.pc += immediate;
//    }
//    // If the action was taken, one additional m-cycle is used.
//    if (condition_met) {
//        return 12;
//    }
//    return 8;
//}

//t_cycle Cpu::save_value_to_address(uint16_t address, uint8_t value) {
//    m_mmu.write_byte(address, value);
//    return 8;
//}
//
//t_cycle Cpu::load_value_from_address(uint16_t address, uint8_t& value) {
//    value = m_mmu.read_byte(address);
//    return 8;
//}

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
    default:
        abort_execution<NotImplementedError>(
            fmt::format("fetch_data: InteractionType {} not implemented",
                        magic_enum::enum_name(current_instruction.interaction_type)));
    }
    return 0;
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

void Cpu::dec8(uint8_t& input) {
    bool hc = internal::was_half_carry(input, 1, std::minus<>());
    set_half_carry_flag(hc ? BitValues::Active : BitValues::Inactive);
    input -= 1;
    set_zero_flag(input == 0 ? BitValues::Active : BitValues::Inactive);
    set_subtract_flag(BitValues::Active);
}

t_cycle Cpu::cp(uint8_t a, uint8_t value) {
    set_zero_flag(a == value ? BitValues::Active : BitValues::Inactive);
    set_subtract_flag(BitValues::Active);

    const auto hc = internal::was_half_carry(a, value, std::minus<>());
    set_half_carry_flag(hc ? BitValues::Active : BitValues::Inactive);

    set_carry_flag(a < value ? BitValues::Active : BitValues::Inactive);
    return 8;
}

t_cycle Cpu::sub(uint8_t value) {
    set_zero_flag(registers.a == value ? BitValues::Active : BitValues::Inactive);
    set_subtract_flag(BitValues::Active);
    set_half_carry_flag(internal::was_half_carry(registers.a, value, std::minus<>())
                            ? BitValues::Active
                            : BitValues::Inactive);
    set_carry_flag(registers.a < value ? BitValues::Active : BitValues::Inactive);
    registers.a -= value;
    return 4;
}

t_cycle Cpu::add(uint8_t value) {
    set_zero_flag(static_cast<int>(registers.a) + static_cast<int>(value) == 256 ? BitValues::Active : BitValues::Inactive);
    set_subtract_flag(BitValues::Inactive);
    set_half_carry_flag(internal::was_half_carry(registers.a, value, std::plus<>())
                            ? BitValues::Active
                            : BitValues::Inactive);
    set_carry_flag(internal::was_carry(registers.a, value, std::plus<>()) ? BitValues::Active : BitValues::Inactive);
    registers.a += value;
    return 4;
}

std::vector<IMemoryRange*> Cpu::get_mappable_memory() {
    return interrupt_handler.get_mappable_memory();
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
    default:
        abort_execution<LogicError>(
            fmt::format("Unknown register value {}", magic_enum::enum_name(register_type)));
        return 0;
    }
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
        auto value = get_register_value(register_);
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
        default:
            abort_execution<LogicError>(
                fmt::format("JR {:02X} could not handle {}", data,
                            magic_enum::enum_name(instruction.condition_type)));
            return false;
        }
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
        default:
            abort_execution<LogicError>(
                fmt::format("CALL {:04X} could not handle {}", data,
                            magic_enum::enum_name(instruction.condition_type)));
            return false;
        }
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
        default:
            abort_execution<LogicError>(fmt::format(
                "RET could not handle {}", magic_enum::enum_name(instruction.condition_type)));
            return false;
        }
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
