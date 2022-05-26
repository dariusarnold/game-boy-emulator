#pragma once

#include "facades/flag.hpp"
#include "facades/register.hpp"

/**
 * The instructions 0x07 RLCA and 0x17 RLA always set the zero flag to inactive, the other rotate
 * left instructions set it if the value was zero.
 */
enum class ZeroFlagSetMode {
    ByResult,
    AlwaysOff,
};

/**
 * This class does both RL and RLC instructions from the extended (0xCB) instruction set.
 * The behaviour is decided by the carry_through_carry_flag value.
 * @tparam R
 */
template <typename R>
class RotateLeft {
    MutableFlag<flags::zero> zero_flag;
    MutableFlag<flags::subtract> subtract_flag;
    MutableFlag<flags::half_carry> half_carry_flag;
    MutableFlag<flags::carry> carry_flag;
    Register<R> register_;
    // If false the rotate is for RLC instructions.
    // C <- [7 <- 0] <- 7
    // If true the rotate is for RL instructions.
    // C <- [7 <- 0] <- C
    bool carry_through_carry_flag = true;
    // The instruction 0x17 RLA doesn't set zero flag even when result is zero.
    ZeroFlagSetMode set_zero_flag_mode = ZeroFlagSetMode::ByResult;

public:
    RotateLeft(MutableFlag<flags::zero> z, MutableFlag<flags::subtract> s,
               MutableFlag<flags::half_carry> hc, MutableFlag<flags::carry> c, Register<R> reg) :
            zero_flag(z), subtract_flag(s), half_carry_flag(hc), carry_flag(c), register_(reg) {}

    RotateLeft(MutableFlag<flags::zero> z, MutableFlag<flags::subtract> s,
               MutableFlag<flags::half_carry> hc, MutableFlag<flags::carry> c, Register<R> reg,
               bool carry_rotated_bit, ZeroFlagSetMode zero_flag_mode) :
            zero_flag(z),
            subtract_flag(s),
            half_carry_flag(hc),
            carry_flag(c),
            register_(reg),
            carry_through_carry_flag(carry_rotated_bit),
            set_zero_flag_mode(zero_flag_mode) {}

    unsigned int execute() {
        if (carry_through_carry_flag) {
            bool cf = carry_flag.read();
            register_.set(bitmanip::rotate_left_carry(register_.get(), cf));
            carry_flag.set(cf);
        } else {
            bool cf = bitmanip::is_bit_set(register_.get(), 7);
            register_.set(bitmanip::rotate_left(register_.get()));
            carry_flag.set(cf);
        }

        if (set_zero_flag_mode == ZeroFlagSetMode::AlwaysOff) {
            zero_flag.set(false);
        } else {
            zero_flag.set(register_.get() == 0x00);
        }
        subtract_flag.set_inactive();
        half_carry_flag.set_inactive();
        return 8;
    }
};
