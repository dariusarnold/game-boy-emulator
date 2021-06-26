#pragma once

#include "facades/flag.hpp"
#include "facades/register.hpp"

template <typename R>
class RotateLeft {
    MutableFlag<flags::zero> zero_flag;
    MutableFlag<flags::subtract> subtract_flag;
    MutableFlag<flags::half_carry> half_carry_flag;
    MutableFlag<flags::carry> carry_flag;
    MutableRegister<R> register_;

public:
    RotateLeft(MutableFlag<flags::zero> z, MutableFlag<flags::subtract> s,
               MutableFlag<flags::half_carry> hc, MutableFlag<flags::carry> c,
               MutableRegister<R> reg) :
            zero_flag(z), subtract_flag(s), half_carry_flag(hc), carry_flag(c), register_(reg) {}

    int execute() {
        bitmanip::rotate_left(register_.get());
        if (bitmanip::is_bit_set(register_.get(), 7)) {
            carry_flag.set_active();
        } else {
            carry_flag.set_inactive();
        }
        if (register_.get() == 0x00) {
            zero_flag.set_active();
        }
        subtract_flag.set_inactive();
        half_carry_flag.set_inactive();
        return 8;
    }
};
