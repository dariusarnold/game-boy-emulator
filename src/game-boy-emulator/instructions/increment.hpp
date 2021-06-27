#pragma once

#include "bitmanipulation.hpp"
#include "constants.h"
#include "cpu.hpp"


template <typename T>
class Increment {
    MutableRegister<T> m_register;
    MutableFlag<flags::zero> zero_flag;
    MutableFlag<flags::subtract> subtract_flag;
    MutableFlag<flags::half_carry> half_carry_flag;
    IncrementPC increment_pc;

    struct ByteRegisterTag {};
    struct WordRegisterTag {};

public:
    Increment(MutableRegister<T> reg, MutableFlag<flags::zero> z_flag,
                   MutableFlag<flags::subtract> s_flag, MutableFlag<flags::half_carry> hc_flag,
                   IncrementPC inc_pc) :
            m_register(reg), zero_flag(z_flag), subtract_flag(s_flag), half_carry_flag(hc_flag),
            increment_pc(inc_pc) {}

    /**
     * Dispatch depending on what register type (single or combined) we are operating on
     * @return
     */
    unsigned int execute() {
        return _execute(typename std::conditional_t<registers::is_byte_register_v<T>, ByteRegisterTag, WordRegisterTag>());
    }

    /**
     * Increment single byte register.
     * @return Number of cycles for execution
     */
    unsigned int _execute(ByteRegisterTag) {
        bool bit_3_before = bitmanip::is_bit_set(m_register.get(), 3);
        m_register.set(m_register.get() + 1);
        bool bit_3_after = bitmanip::is_bit_set(m_register.get(), 3);
        if (bit_3_before and not bit_3_after) {
            half_carry_flag.set_active();
        } else {
            half_carry_flag.set_inactive();
        }
        if (m_register.get() == 0) zero_flag.set_active();
        subtract_flag.set_inactive();
        increment_pc.increment();
        return 4;
    }

    /**
     * Increment word register.
     * @return Number of cycles for execution
     */
    unsigned int _execute(WordRegisterTag) {
        bool bit_11_before = bitmanip::is_bit_set(m_register.get(), 11);
        m_register.set(m_register.get() + 1);
        bool bit_11_after = bitmanip::is_bit_set(m_register.get(), 11);
        if (bit_11_before and not bit_11_after) {
            half_carry_flag.set_active();
        } else {
            half_carry_flag.set_inactive();
        }
        if (m_register.get() == 0) zero_flag.set_active();
        subtract_flag.set_inactive();
        increment_pc.increment();
        return 8;
    }
};
