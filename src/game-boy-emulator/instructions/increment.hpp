#pragma once

#include "bitmanipulation.hpp"
#include "constants.h"
#include "cpu.hpp"


template <typename T>
class Increment {
    Register<T> m_register;
    MutableFlag<flags::zero> zero_flag;
    MutableFlag<flags::subtract> subtract_flag;
    MutableFlag<flags::half_carry> half_carry_flag;

    struct ByteRegisterTag {};
    struct WordRegisterTag {};

public:
    Increment(Register<T> reg, MutableFlag<flags::zero> z_flag,
                   MutableFlag<flags::subtract> s_flag, MutableFlag<flags::half_carry> hc_flag) :
            m_register(reg), zero_flag(z_flag), subtract_flag(s_flag), half_carry_flag(hc_flag) {}

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
        half_carry_flag.set(bit_3_before and not bit_3_after);
        zero_flag.set(m_register.get() == 0);
        subtract_flag.set_inactive();
        return 4;
    }

    /**
     * Increment word register.
     * @return Number of cycles for execution
     */
    unsigned int _execute(WordRegisterTag) {
        m_register.set(m_register.get() + 1);
        return 8;
    }
};
