#pragma once

#include "bitmanipulation.hpp"
#include "constants.h"
#include "cpu.hpp"


template <typename T>
class IncrementByte {
    Register<T> m_register;
    MutableFlag<flags::zero> zero_flag;
    MutableFlag<flags::subtract> subtract_flag;
    MutableFlag<flags::half_carry> half_carry_flag;

public:
    IncrementByte(Register<T> reg, MutableFlag<flags::zero> z_flag,
                  MutableFlag<flags::subtract> s_flag, MutableFlag<flags::half_carry> hc_flag) :
            m_register(reg), zero_flag(z_flag), subtract_flag(s_flag), half_carry_flag(hc_flag) {}

    /**
     * Increment single byte register.
     * @return Number of cycles for execution
     */
    unsigned int execute() {
        bool bit_3_before = bitmanip::is_bit_set(m_register.get(), 3);
        m_register.set(m_register.get() + 1);
        bool bit_3_after = bitmanip::is_bit_set(m_register.get(), 3);
        half_carry_flag.set(bit_3_before and not bit_3_after);
        zero_flag.set(m_register.get() == 0);
        subtract_flag.set_inactive();
        return 4;
    }
};

template <typename T>
class IncrementWord {
    Register<T> m_register;

public:
    IncrementWord(Register<T> reg): m_register(reg) {}

    /**
     * Increment word register.
     * @return Number of cycles for execution
     */
    unsigned int execute() {
        m_register.set(m_register.get() + 1);
        return 8;
    }
};
