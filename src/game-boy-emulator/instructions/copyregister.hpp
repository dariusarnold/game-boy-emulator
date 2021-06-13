#pragma once

#include "facades/register.hpp"
#include "facades/incrementpc.hpp"

/**
 * Copy value from input register to output register.
 * @tparam I
 * @tparam O
 */
template <typename I, typename O>
class CopyRegister {
    // Check if same register type
    static_assert(is_byte_register_v<I> and is_byte_register_v<O> or is_word_register_v<I> and is_word_register_v<O>);
    Register<I> source;
    MutableRegister<O> destination;
    IncrementPC increment_pc;

public:
    CopyRegister(Register<I> sourc, MutableRegister<O> dest, IncrementPC ipc) :
            source(sourc), destination(dest), increment_pc(ipc) {
    }

    int execute() {
        destination.set(source.get());
        increment_pc.increment();
        return 8;
    }
};
