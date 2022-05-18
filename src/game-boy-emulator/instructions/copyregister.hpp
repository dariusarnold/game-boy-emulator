#pragma once

#include "facades/programcounter.hpp"
#include "facades/register.hpp"

/**
 * Copy value from input register to output register.
 * @tparam I
 * @tparam O
 */
template <typename I, typename O>
class CopyRegister {
    // Check if same register type
    static_assert((registers::is_byte_register_v<I> and registers::is_byte_register_v<O>)
                  or (registers::is_word_register_v<I> and registers::is_word_register_v<O>));
    Register<I> source;
    Register<O> destination;

public:
    CopyRegister(Register<I> sourc, Register<O> dest) :
            source(sourc), destination(dest) {}

    unsigned int execute() {
        destination.set(source.get());
        return 8;
    }
};
