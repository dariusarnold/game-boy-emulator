#pragma once

#include "constants.h"
#include "bitmanipulation.hpp"

#include <cstdint>

/**
 * Facade for reading flags.
 * @tparam f Which flag this facade is watching.
 */
template <flags f>
class Flag {
    uint8_t flag_register;

public:
    explicit Flag(uint8_t f_reg) : flag_register(f_reg){};

    bool read() {
        return bitmanip::is_bit_set(flag_register, as_integral(f));
    }
};

/**
 * Facade for setting and reading flags
 * @tparam f Which flag this facade is watching
 */
template <flags f>
class MutableFlag {
    uint8_t& flag_register;

public:
    explicit MutableFlag(uint8_t& f_reg) : flag_register(f_reg){};

    bool read() {
        return bitmanip::is_bit_set(flag_register, as_integral(f));
    }
    void set_active() {
        bitmanip::set(flag_register, as_integral(f));
    }
    void set_inactive() {
        bitmanip::unset(flag_register, as_integral(f));
    }
};