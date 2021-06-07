#pragma once

#include "constants.h"
#include <cstdint>

namespace bitmanip {
     /**
      * Set bit at position to value
      * @param input Byte to modify
      * @param position 0..7 where 0 is the least significant bit
      * @param value 0..1
      */
    void set(uint8_t& input, uint8_t position, BitValues value);
    // Set bit in input at position to 1
    void set(uint8_t& input, uint8_t position);
    // Set bit in input at position to 0
    void unset(uint8_t& input, uint8_t position);

} // namespace bitmanip