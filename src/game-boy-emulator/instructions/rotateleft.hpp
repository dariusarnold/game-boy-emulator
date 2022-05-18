#pragma once

#include "facades/flag.hpp"
#include "facades/register.hpp"

template <typename R>
class RotateLeft {
    MutableFlag<flags::zero> zero_flag;
    MutableFlag<flags::subtract> subtract_flag;
    MutableFlag<flags::half_carry> half_carry_flag;
    MutableFlag<flags::carry> carry_flag;
    Register<R> register_;
    // If true, the bit which was rotated out is appended on the right side again.
    // If false, 0 will be appended.
    bool carry_bit = false;

public:
    RotateLeft(MutableFlag<flags::zero> z, MutableFlag<flags::subtract> s,
               MutableFlag<flags::half_carry> hc, MutableFlag<flags::carry> c, Register<R> reg, bool carry_rotated_bit) :
            zero_flag(z), subtract_flag(s), half_carry_flag(hc), carry_flag(c), register_(reg), carry_bit(carry_rotated_bit) {}

    RotateLeft(MutableFlag<flags::zero> z, MutableFlag<flags::subtract> s,
               MutableFlag<flags::half_carry> hc, MutableFlag<flags::carry> c, Register<R> reg) :
            RotateLeft(z, s, hc, c, reg, false) {}

    unsigned int execute() {
        if (bitmanip::is_bit_set(register_.get(), 7)) {
            carry_flag.set_active();
        } else {
            carry_flag.set_inactive();
        }
        if (carry_bit) {
            register_.set(bitmanip::rotate_left_carry(register_.get()));
        } else {
            register_.set(bitmanip::rotate_left(register_.get()));
        }
        if (register_.get() == 0x00) {
            zero_flag.set_active();
        } else {
            zero_flag.set_inactive();
        }
        subtract_flag.set_inactive();
        half_carry_flag.set_inactive();
        return 8;
    }
};
